let interTimer = null;
let isInteracting = false;

function updateUI() {
    if (isInteracting) return;

    fetch('/api/status')
    .then(res => res.json())
    .then(st => {
        // Температуры
        const tempCurrEl = document.getElementById('temp-curr');
        const tempTargetValEl = document.getElementById('temp-target-val');
        const sliderEl = document.getElementById('temp-slider');

        if (tempCurrEl) tempCurrEl.innerText = st.temp_curr.toFixed(1);
        if (tempTargetValEl) tempTargetValEl.innerText = st.temp_target.toFixed(1);
        if (sliderEl) sliderEl.value = st.temp_target;

        // Связь
        const badge = document.getElementById('status-badge');
        if (badge) {
            badge.innerText = st.alive ? "Online" : "Offline";
            badge.className = "badge " + (st.alive ? "online" : "");
        }
   
        // Карточки
        const relayEl = document.getElementById('relay-status');
        if (relayEl) {
            relayEl.innerText = st.relay ? "ГРЕЕТ" : "ПАУЗА";
            relayEl.style.color = st.relay ? "#FF9800" : "#2196F3";
        }
        
        const sensorEl = document.getElementById('sensor-status');
        if (sensorEl) sensorEl.innerText = st.sensor_ok ? "OK" : "ОШИБКА";

        const uptimeEl = document.getElementById('uptime');
        if (uptimeEl) uptimeEl.innerText = Math.floor(st.uptime / 60) + "м";
   
        // Кольцо
        const gauge = document.getElementById('main-gauge');
        if (gauge) {
            gauge.classList.remove('heating', 'emergency');
            if (!st.sensor_ok || !st.alive) {
                gauge.classList.add('emergency');
            } else if (st.relay) {
                gauge.classList.add('heating');
            }
        }
    })
    .catch(err => console.log("Fetch error:", err));
}

function sendTarget(val) {
    fetch(`/api/set_temp?val=${val}`).catch(e => console.error("Send error:", e));
}

const slider = document.getElementById('temp-slider');
if (slider) {
    slider.oninput = () => {
        isInteracting = true;
        clearTimeout(interTimer);
        const valDisplay = document.getElementById('temp-target-val');
        if (valDisplay) valDisplay.innerText = parseFloat(slider.value).toFixed(1);
    };

    slider.onchange = () => {
        sendTarget(slider.value);
        interTimer = setTimeout(() => { isInteracting = false; }, 4000);
    };
}

function setPreset(v) {
    isInteracting = true;
    clearTimeout(interTimer);
    const valDisplay = document.getElementById('temp-target-val');
    const sliderEl = document.getElementById('temp-slider');
    
    if (valDisplay) valDisplay.innerText = v.toFixed(1);
    if (sliderEl) sliderEl.value = v;
    
    sendTarget(v);
    interTimer = setTimeout(() => { isInteracting = false; }, 4000);
}

setInterval(updateUI, 1500);
updateUI();

#pragma once
#include <pgmspace.h>

const char INDEX_HTML[] PROGMEM = R"HTMLEOF(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>FIREWATCH-01 :: Fire Detection &amp; Suppression Monitor</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Big+Shoulders+Display:wght@700;900&family=IBM+Plex+Mono:wght@400;500;600&display=swap" rel="stylesheet">
<style>
  :root{
    --bg:#14171a; --panel:#1d2125; --panel-2:#262b2f; --line:#343a3f;
    --text:#ede9e4; --text-dim:#8b9298;
    --red:#e8463c; --amber:#f2a93b; --teal:#57d6c4;
    --font-display:'Big Shoulders Display', sans-serif;
    --font-mono:'IBM Plex Mono', 'Courier New', monospace;
  }
  *{ margin:0; padding:0; box-sizing:border-box; }
  html,body{ background:var(--bg); color:var(--text); }
  body{ font-family:var(--font-mono); padding:20px; max-width:1280px; margin:0 auto; line-height:1.4; }
  button, input{ font-family:inherit; }

  .topbar{
    display:flex; flex-wrap:wrap; gap:16px;
    justify-content:space-between; align-items:center;
    padding-bottom:16px; margin-bottom:24px;
    border-bottom:4px solid var(--line); position:relative;
  }
  .brand{ display:flex; align-items:flex-start; gap:10px; }
  .brand-mark{ color:var(--red); font-size:1.4rem; line-height:1; }
  .brand h1{ font-family:var(--font-display); font-weight:900; font-size:2.1rem; letter-spacing:0.02em; text-transform:uppercase; line-height:1; }
  .brand-id{ color:var(--text-dim); }
  .brand-tagline{ font-size:0.68rem; letter-spacing:0.15em; color:var(--text-dim); text-transform:uppercase; margin-top:4px; }
  .status-cluster{ display:flex; align-items:center; gap:18px; flex-wrap:wrap; }
  .conn-indicator{ display:flex; align-items:center; gap:8px; font-size:0.75rem; letter-spacing:0.08em; color:var(--text-dim); text-transform:uppercase; }
  .conn-indicator .dot{ width:8px; height:8px; border-radius:50%; background:var(--text-dim); }
  .conn-indicator.live .dot{ background:var(--teal); box-shadow:0 0 6px var(--teal); }
  .beacon{ width:18px; height:18px; border-radius:50%; background:var(--panel-2); border:2px solid var(--line); }
  .system-status{ font-family:var(--font-display); font-weight:700; font-size:0.95rem; letter-spacing:0.1em; padding:6px 16px; border:2px solid var(--teal); color:var(--teal); text-transform:uppercase; }

  body.alarm .beacon{ background:var(--red); border-color:var(--red); animation:pulse 1s infinite; }
  body.alarm .system-status{ border-color:var(--red); color:var(--red); animation:flicker 1s infinite; }
  body.alarm .topbar::after{ content:''; position:absolute; left:0; right:0; bottom:-4px; height:4px; background:repeating-linear-gradient(45deg, var(--amber) 0 10px, var(--red) 10px 20px); animation:hazard 1s linear infinite; }
  @keyframes pulse{ 0%,100%{ box-shadow:0 0 6px 2px var(--red); } 50%{ box-shadow:0 0 16px 8px var(--red); opacity:0.7; } }
  @keyframes flicker{ 0%,100%{ opacity:1; } 50%{ opacity:0.55; } }
  @keyframes hazard{ from{ background-position:0 0; } to{ background-position:28px 0; } }

  .panel{ background:var(--panel); border:1px solid var(--line); padding:16px; }
  .panel-eyebrow{ display:flex; justify-content:space-between; gap:8px; font-size:0.68rem; letter-spacing:0.14em; color:var(--text-dim); text-transform:uppercase; margin-bottom:10px; }

  .grid-top{ display:grid; grid-template-columns:1.3fr 1fr; gap:16px; margin-bottom:16px; }
  @media (max-width:840px){ .grid-top{ grid-template-columns:1fr; } }

  .flame-card{ display:flex; flex-direction:column; justify-content:center; gap:12px; }
  .flame-status{ font-family:var(--font-display); font-weight:900; font-size:3rem; text-transform:uppercase; line-height:1; }
  .flame-status.clear{ color:var(--teal); }
  .flame-status.detected{ color:var(--red); }
  .flame-sub{ font-size:0.75rem; color:var(--text-dim); letter-spacing:0.06em; }

  .grid-sensors{ display:grid; grid-template-columns:repeat(auto-fit, minmax(190px,1fr)); gap:16px; margin-bottom:16px; }
  .readout{ font-family:var(--font-mono); font-weight:600; font-size:1.9rem; }
  .readout .unit{ font-size:0.95rem; color:var(--text-dim); margin-left:4px; font-weight:400; }
  .bar-track{ position:relative; height:8px; background:var(--panel-2); margin-top:12px; overflow:hidden; }
  .bar-fill{ height:100%; width:0%; background:var(--teal); transition:width 0.6s ease, background 0.3s ease; }
  .bar-fill.warn{ background:var(--amber); }
  .bar-fill.danger{ background:var(--red); }
  .bar-scale{ display:flex; justify-content:space-between; font-size:0.62rem; color:var(--text-dim); letter-spacing:0.1em; text-transform:uppercase; margin-top:6px; }

  .climate-row{ display:flex; gap:20px; }
  .climate-item .readout{ font-size:1.7rem; }
  .climate-item .label{ font-size:0.65rem; color:var(--text-dim); letter-spacing:0.12em; text-transform:uppercase; margin-top:6px; }

  .water-row{ display:flex; justify-content:space-between; align-items:center; gap:12px; }
  .tank{ position:relative; width:34px; height:84px; border:2px solid var(--line); background:var(--panel-2); flex:none; }
  .tank-fill{ position:absolute; bottom:0; left:0; right:0; height:0%; background:linear-gradient(to top, var(--teal), #3aa89a); transition:height 0.6s ease; }
  .tank-ticks{ position:absolute; inset:0; background:repeating-linear-gradient(to top, transparent 0 19px, var(--line) 19px 20px); pointer-events:none; }

  .section-label{ font-size:0.7rem; letter-spacing:0.16em; color:var(--text-dim); text-transform:uppercase; margin-bottom:10px; }
  .actuator-bank{ display:grid; grid-template-columns:repeat(auto-fit, minmax(220px,1fr)); gap:12px; margin-bottom:16px; }
  .actuator-item{ display:flex; justify-content:space-between; align-items:center; gap:12px; }
  .actuator-item .name{ font-size:0.68rem; letter-spacing:0.1em; color:var(--text-dim); text-transform:uppercase; margin-bottom:6px; }
  .actuator-item .value{ font-weight:600; font-size:1.15rem; }
  .toggle-btn{ background:var(--panel-2); border:1px solid var(--line); color:var(--text); font-size:0.7rem; padding:8px 14px; cursor:pointer; text-transform:uppercase; letter-spacing:0.08em; transition:border-color 0.2s, color 0.2s, background 0.2s; }
  .toggle-btn:hover{ border-color:var(--teal); color:var(--teal); }
  .toggle-btn:focus-visible{ outline:2px solid var(--teal); outline-offset:2px; }
  .toggle-btn.on{ background:var(--teal); color:var(--bg); border-color:var(--teal); }
  .slider{ width:120px; accent-color:var(--teal); }

  .log-panel{ margin-bottom:16px; }
  .log-list{ list-style:none; font-size:0.78rem; color:var(--text-dim); display:flex; flex-direction:column; gap:6px; max-height:150px; overflow-y:auto; }
  .log-list .time{ color:var(--text); margin-right:10px; }

  .footer-bar{ display:flex; justify-content:space-between; align-items:center; flex-wrap:wrap; gap:12px; font-size:0.78rem; color:var(--text-dim); padding-top:12px; border-top:1px solid var(--line); }
  .footer-bar a{ color:var(--teal); text-decoration:none; border-bottom:1px dotted var(--teal); }

  @media (prefers-reduced-motion: reduce){ *, *::before, *::after{ animation:none !important; transition:none !important; } }
</style>
</head>
<body>

  <header class="topbar">
    <div class="brand">
      <span class="brand-mark">&#9670;</span>
      <div>
        <h1>FIREWATCH<span class="brand-id">-01</span></h1>
        <div class="brand-tagline">Fire Detection &amp; Auto-Suppression Monitor</div>
      </div>
    </div>
    <div class="status-cluster">
      <div class="conn-indicator" id="connIndicator">
        <span class="dot"></span><span>LINK: <span id="connText">SIMULATED</span></span>
      </div>
      <div class="beacon" id="beacon"></div>
      <div class="system-status" id="systemStatus">NORMAL</div>
    </div>
  </header>

  <div class="grid-top">
    <div class="panel">
      <div class="panel-eyebrow"><span>CH-00</span><span>CAMERA</span></div>
      <div id="camConnect" style="margin-bottom:8px">
        <input id="camIP" placeholder="ESP32-CAM IP" style="background:var(--panel-2);border:1px solid var(--line);color:var(--text);padding:6px;font-family:var(--font-mono);font-size:0.75rem;width:180px">
        <button onclick="var ip=document.getElementById('camIP').value;if(ip){document.getElementById('camImg').src='http://'+ip+':81/stream';document.getElementById('camConnect').style.display='none'}" style="background:var(--teal);color:var(--bg);border:none;padding:6px 12px;cursor:pointer;font-family:var(--font-mono);font-size:0.75rem">CONNECT</button>
      </div>
      <img id="camImg" src="" style="width:100%;object-fit:contain;">
    </div>
    <div class="panel flame-card">
      <div class="panel-eyebrow"><span>CH-01</span><span>FLAME</span></div>
      <div class="flame-status clear" id="flameStatus">CLEAR</div>
      <div class="flame-sub">DIGITAL FLAME SENSOR — TRIGGERS IMMEDIATE ALERT</div>
    </div>
  </div>

  <div class="grid-sensors">
    <div class="panel">
      <div class="panel-eyebrow"><span>CH-02</span><span>SMOKE (MQ-2)</span></div>
      <div class="readout"><span id="smokeValue">0</span><span class="unit">/ 1023</span></div>
      <div class="bar-track"><div class="bar-fill" id="smokeBar"></div></div>
      <div class="bar-scale"><span>CLEAR</span><span>DANGER</span></div>
    </div>
    <div class="panel">
      <div class="panel-eyebrow"><span>CH-03</span><span>CLIMATE (DHT11)</span></div>
      <div class="climate-row">
        <div class="climate-item">
          <div class="readout"><span id="tempValue">0.0</span><span class="unit">&deg;C</span></div>
          <div class="label">Temp</div>
        </div>
        <div class="climate-item">
          <div class="readout"><span id="humidValue">0</span><span class="unit">%</span></div>
          <div class="label">RH</div>
        </div>
      </div>
    </div>
    <div class="panel">
      <div class="panel-eyebrow"><span>CH-04</span><span>WATER (HC-SR04)</span></div>
      <div class="water-row">
        <div class="readout"><span id="waterValue">0</span><span class="unit">%</span></div>
        <div class="tank">
          <div class="tank-fill" id="waterFill"></div>
          <div class="tank-ticks"></div>
        </div>
      </div>
    </div>
    <div class="panel">
      <div class="panel-eyebrow"><span>CH-06</span><span>AMBIENT LIGHT (LDR)</span></div>
      <div class="readout"><span id="ldrValue">0</span><span class="unit">/ 1023</span></div>
      <div class="bar-track"><div class="bar-fill" id="ldrBar" style="background:var(--amber)"></div></div>
      <div class="bar-scale"><span>DARK</span><span>BRIGHT</span></div>
    </div>
  </div>

  <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:10px">
    <div class="section-label" style="margin-bottom:0">Output channels — <span id="modeLabel" style="color:var(--teal)">AUTO</span></div>
    <button onclick="fetch('/control',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({mode:'auto'})});document.getElementById('modeLabel').textContent='AUTO';document.getElementById('modeLabel').style.color='var(--teal)'" style="background:transparent;border:1px solid var(--teal);color:var(--teal);padding:4px 12px;cursor:pointer;font-family:var(--font-mono);font-size:0.65rem;letter-spacing:0.1em;text-transform:uppercase">RESET TO AUTO</button>
  </div>
  <div class="actuator-bank">
    <div class="panel actuator-item">
      <div>
        <div class="name">OUT-01 :: PUMP (RELAY)</div>
        <div class="value" id="pumpValue">OFF</div>
      </div>
      <button class="toggle-btn" id="pumpBtn">TOGGLE</button>
    </div>
    <div class="panel actuator-item">
      <div>
        <div class="name">OUT-03 :: NOZZLE SERVO</div>
        <div class="value" id="servoValue">90&deg;</div>
      </div>
      <input type="range" class="slider" id="servoSlider" min="0" max="180" value="90">
    </div>
    <div class="panel actuator-item">
      <div>
        <div class="name">OUT-04 :: BUZZER</div>
        <div class="value" id="buzzerValue">OFF</div>
      </div>
      <button class="toggle-btn" id="buzzerBtn">TOGGLE</button>
    </div>
  </div>

  <div class="panel log-panel">
    <div class="panel-eyebrow"><span>EVENT LOG</span><span id="logCount">0</span></div>
    <ul class="log-list" id="logList"></ul>
  </div>

  <div class="footer-bar">
    <a id="historyLink" href="#" target="_blank" rel="noopener">Historical data (ThingSpeak) &#8599;</a>
  </div>

<script>
  const DATA_ENDPOINT = '/data';
  const CONTROL_ENDPOINT = '/control';
  const POLL_INTERVAL = 2000;
  const THINGSPEAK_CHANNEL_URL = 'https://thingspeak.mathworks.com/channels/3418098';
  const THRESH = { smoke: 600, smokeMax: 1023, temp: 50 };
  const MAX_LOG = 8;

  let state = {
    flame: 0, smoke: 0, temp: 0, humidity: 0, waterLevel: 0,
    ldr: 0, pump: 0, buzzer: 0, servo: 90
  };
  let prevAlarm = false, prevFlame = 0;

  document.getElementById('historyLink').href = THINGSPEAK_CHANNEL_URL;

  const clamp = (v,a,b) => Math.min(b, Math.max(a, v));
  const logList = document.getElementById('logList');
  const logCount = document.getElementById('logCount');
  let logTotal = 0;

  function logEvent(msg){
    const time = new Date().toLocaleTimeString('en-GB', { hour12:false });
    const li = document.createElement('li');
    li.innerHTML = '<span class="time">' + time + '</span>' + msg;
    logList.prepend(li);
    while (logList.children.length > MAX_LOG) logList.removeChild(logList.lastChild);
    logTotal++;
    logCount.textContent = logTotal;
  }

  function evaluateAlarm(){
    return state.flame === 1 || state.smoke > THRESH.smoke || state.temp > THRESH.temp;
  }

  async function fetchData(){
    let live = false;
    try {
      const res = await fetch(DATA_ENDPOINT, { cache:'no-store' });
      if (res.ok){
        const data = await res.json();
        state = { ...state, ...data };
        live = true;
      }
    } catch (e){}

    const alarm = evaluateAlarm();
    if (alarm !== prevAlarm) logEvent('SYSTEM STATUS: ' + (alarm ? 'NORMAL &rarr; ALERT' : 'ALERT &rarr; NORMAL'));
    if (state.flame !== prevFlame) logEvent('CH-01 FLAME: ' + (state.flame ? 'CLEAR &rarr; DETECTED' : 'DETECTED &rarr; CLEAR'));
    prevAlarm = alarm; prevFlame = state.flame;
    render(alarm, live);
  }

  function render(alarm, live){
    document.body.classList.toggle('alarm', alarm);
    document.getElementById('systemStatus').textContent = alarm ? 'ALERT' : 'NORMAL';
    document.getElementById('connIndicator').classList.toggle('live', live);
    document.getElementById('connText').textContent = live ? 'ESP8266' : 'OFFLINE';

    const flameEl = document.getElementById('flameStatus');
    flameEl.textContent = state.flame ? 'DETECTED' : 'CLEAR';
    flameEl.className = 'flame-status ' + (state.flame ? 'detected' : 'clear');

    document.getElementById('smokeValue').textContent = Math.round(state.smoke);
    const smokeBar = document.getElementById('smokeBar');
    smokeBar.style.width = clamp(state.smoke / THRESH.smokeMax * 100, 0, 100) + '%';
    smokeBar.className = 'bar-fill' + (state.smoke > THRESH.smoke ? ' danger' : state.smoke > THRESH.smoke*0.6 ? ' warn' : '');

    document.getElementById('tempValue').textContent = state.temp.toFixed(1);
    document.getElementById('humidValue').textContent = Math.round(state.humidity);

    document.getElementById('waterFill').style.height = state.waterLevel + '%';
    document.getElementById('waterValue').textContent = Math.round(state.waterLevel);

    document.getElementById('ldrValue').textContent = Math.round(state.ldr);
    document.getElementById('ldrBar').style.width = clamp(state.ldr/1023*100, 0, 100) + '%';

    setActuator('pump', state.pump);
    setActuator('buzzer', state.buzzer);
    document.getElementById('servoValue').innerHTML = state.servo + '&deg;';
    document.getElementById('servoSlider').value = state.servo;
  }

  function setActuator(name, on){
    document.getElementById(name + 'Value').textContent = on ? 'ON' : 'OFF';
    document.getElementById(name + 'Btn').classList.toggle('on', !!on);
  }

  async function sendControl(payload){
    try {
      await fetch(CONTROL_ENDPOINT, {
        method:'POST',
        headers:{ 'Content-Type':'application/json' },
        body: JSON.stringify(payload)
      });
    } catch (e){}
  }

  function toggleActuator(name){
    state[name] = state[name] ? 0 : 1;
    sendControl({ [name]: state[name] });
    render(evaluateAlarm(), false);
    document.getElementById('modeLabel').textContent = 'MANUAL';
    document.getElementById('modeLabel').style.color = 'var(--amber)';
    logEvent('MANUAL: ' + name.toUpperCase() + ' &rarr; ' + (state[name] ? 'ON' : 'OFF'));
  }

  document.getElementById('pumpBtn').addEventListener('click', () => toggleActuator('pump'));
  document.getElementById('buzzerBtn').addEventListener('click', () => toggleActuator('buzzer'));

  document.getElementById('servoSlider').addEventListener('input', (e) => {
    state.servo = parseInt(e.target.value, 10);
    document.getElementById('servoValue').innerHTML = state.servo + '&deg;';
    sendControl({ servo: state.servo });
    document.getElementById('modeLabel').textContent = 'MANUAL';
    document.getElementById('modeLabel').style.color = 'var(--amber)';
  });

  fetchData();
  setInterval(fetchData, POLL_INTERVAL);
</script>

</body>
</html>
)HTMLEOF";

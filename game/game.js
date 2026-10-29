(() => {
  const $ = id => document.getElementById(id);
  const stage = $("stage"), speaker = $("speaker"), text = $("text"), controls = $("controls");
  const SAVE = "solsticio-save";
  let state = {mission: 0, step: 0, bond: 0, flags: {}};
  let cleanup = null, gm = null;

  const save = () => localStorage.setItem(SAVE, JSON.stringify(state));
  const setBond = () => $("bond-val").textContent = state.bond;
  const clearStage = () => { if (cleanup) cleanup(); cleanup = null; };
  const btn = (label, fn, cls) => {
    const b = document.createElement("button");
    b.textContent = label; if (cls) b.className = cls; b.onclick = fn; controls.appendChild(b); return b;
  };
  const say = (who, msg) => { speaker.textContent = who; text.textContent = msg; };

  function title() {
    clearStage();
    $("mission-title").textContent = "";
    stage.innerHTML = "<h1>SOLSTÍCIO</h1><div class='place'>Ecos do Tempo</div><div class='hint'>Aventura de ficção científica · ~5 horas · 12 missões</div>";
    say("", "Uma armadura que viaja no tempo com o calor do Sol. Uma escolha sobre o que torna uma relação verdadeira.");
    controls.innerHTML = "";
    btn("Nova campanha", () => { state = {mission: 0, step: 0, bond: 0, flags: {}}; setBond(); startMission(); }, "primary");
    if (localStorage.getItem(SAVE)) btn("Continuar", () => { state = JSON.parse(localStorage.getItem(SAVE)); setBond(); startMission(true); });
  }

  function startMission(resume) {
    const m = MISSIONS[state.mission];
    if (!m) return ending();
    $("mission-title").textContent = m.title;
    if (!resume) state.step = 0;
    stage.innerHTML = `<h1>${m.title}</h1><div class='place'>${m.place}</div><div class='hint'>~${m.minutes} min</div>`;
    save(); runStep();
  }

  function next() { state.step++; save(); runStep(); }

  function showLines(lines, done) {
    let i = 0;
    const go = () => {
      if (i >= lines.length) return done();
      const [w, t] = lines[i++];
      say(w, t); controls.innerHTML = ""; btn("Continuar ▸", go, "primary");
    };
    go();
  }

  function runStep() {
    clearStage(); if (gm) { gm.remove(); gm = null; } controls.innerHTML = "";
    const m = MISSIONS[state.mission];
    if (state.step >= m.steps.length) {
      say("", `${m.title} concluída.`);
      btn(state.mission + 1 < MISSIONS.length ? "Próxima missão ▸" : "Ver final ▸", () => { state.mission++; state.step = 0; save(); startMission(); }, "primary");
      return;
    }
    const s = m.steps[state.step];
    if (Array.isArray(s)) { say(s[0], s[1]); btn("Continuar ▸", next, "primary"); }
    else if (s.choice) doChoice(s.choice);
    else if (s.game) doGame(s);
  }

  function doChoice(c) {
    say("", c.prompt);
    c.options.forEach(o => btn(o.t, () => {
      state.bond += o.bond || 0; setBond();
      controls.innerHTML = "";
      showLines(o.lines, next);
    }));
  }

  function doGame(g) {
    say("Desafio", g.prompt);
    gm = document.createElement("div"); gm.style.cssText = "width:100%;display:flex;flex-direction:column;align-items:center;gap:10px"; stage.appendChild(gm);
    ({sequence, timing, slider, targets})[g.game](g);
  }

  function sequence(g) {
    const box = document.createElement("div"); box.style.cssText = "display:flex;flex-wrap:wrap;gap:8px;justify-content:center";
    const info = document.createElement("div"); info.className = "hint";
    gm.append(box, info);
    let order = [];
    const items = g.items.slice();
    items.forEach(name => {
      const b = document.createElement("button"); b.className = "mod"; b.textContent = name; box.appendChild(b);
      b.onclick = () => {
        if (g.ordered && g.ordered[order.length] !== name) {
          info.textContent = "Sequência errada — o sistema reinicia."; order = [];
          box.querySelectorAll("button").forEach(x => x.classList.remove("done")); return;
        }
        order.push(name); b.classList.add("done"); info.textContent = `${order.length}/${items.length}`;
        if (order.length === items.length) {
          if (g.key) state.flags[g.key] = name;
          info.textContent = "Concluído!"; controls.innerHTML = ""; btn("Continuar ▸", next, "primary");
        }
      };
    });
  }

  function timing(g) {
    const bar = document.createElement("div"); bar.className = "bar";
    const start = Math.random() * (1 - g.width - 0.1) + 0.05;
    bar.innerHTML = `<div class='zone' style='left:${start * 100}%;width:${g.width * 100}%'></div><div class='cursor'></div>`;
    const info = document.createElement("div"); info.className = "hint";
    gm.append(bar, info);
    const cur = bar.querySelector(".cursor");
    let t0 = performance.now(), raf, pos = 0, stopped = false;
    const loop = now => {
      pos = Math.abs(((now - t0) / 1000 * g.speed) % 2 - 1);
      cur.style.left = `calc(${pos * 100}% - 3px)`;
      raf = requestAnimationFrame(loop);
    };
    raf = requestAnimationFrame(loop);
    cleanup = () => cancelAnimationFrame(raf);
    controls.innerHTML = "";
    btn("Agora!", () => {
      if (stopped) return;
      if (pos >= start && pos <= start + g.width) {
        stopped = true; cancelAnimationFrame(raf); info.textContent = "Perfeito!";
        controls.innerHTML = ""; btn("Continuar ▸", next, "primary");
      } else info.textContent = "Falhaste — tenta outra vez.";
    }, "primary");
  }

  function slider(g) {
    const r = document.createElement("input"); r.type = "range"; r.min = 0; r.max = 1000; r.value = 0;
    const info = document.createElement("div"); info.className = "hint"; info.textContent = "Luz: 0%";
    gm.append(r, info);
    const intensity = v => Math.max(0, 1 - Math.abs(v - g.target) / 0.4);
    r.oninput = () => info.textContent = `Luz: ${Math.round(intensity(r.value / 1000) * 100)}%`;
    controls.innerHTML = "";
    btn("Fixar espelho", () => {
      if (Math.abs(r.value / 1000 - g.target) <= g.tol) {
        r.disabled = true; info.textContent = "Núcleo carregado!"; controls.innerHTML = ""; btn("Continuar ▸", next, "primary");
      } else info.textContent += " — ainda não está alinhado.";
    }, "primary");
  }

  function targets(g) {
    const field = document.createElement("div"); field.className = "field";
    const info = document.createElement("div"); info.className = "hint";
    gm.append(field, info);
    let left = g.count, time = g.time, over = false;
    const upd = () => info.textContent = `Drones: ${left} · Tempo: ${time}s`;
    const spawn = () => {
      const b = document.createElement("button"); b.className = "target"; b.textContent = "◎";
      const place = () => { b.style.left = Math.random() * (field.clientWidth - 48) + "px"; b.style.top = Math.random() * (field.clientHeight - 48) + "px"; };
      place(); field.appendChild(b);
      b.onclick = () => { b.remove(); left--; upd(); if (left <= 0) win(); };
      b._mv = setInterval(place, 1100);
    };
    for (let i = 0; i < g.count; i++) spawn();
    const win = () => { over = true; clearInterval(timer); field.innerHTML = ""; info.textContent = "Drones desativados!"; controls.innerHTML = ""; btn("Continuar ▸", next, "primary"); };
    const timer = setInterval(() => {
      if (over) return;
      time--; upd();
      if (time <= 0) { clearInterval(timer); runStep(); }
    }, 1000);
    upd();
    cleanup = () => { clearInterval(timer); field.querySelectorAll("button").forEach(b => clearInterval(b._mv)); };
  }

  function ending() {
    const e = ENDINGS.find(x => state.bond >= x.min);
    $("mission-title").textContent = "Fim";
    stage.innerHTML = `<h1>${e.title}</h1><div class='place'>Ligação Tim–Íris: ${state.bond}</div>`;
    say("Epílogo", e.text);
    controls.innerHTML = "";
    btn("Voltar ao início", () => { localStorage.removeItem(SAVE); title(); }, "primary");
  }

  setBond(); title();
})();


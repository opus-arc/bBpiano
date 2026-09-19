'use strict';
const folio = document.querySelector('.folio');
const frames = [...document.querySelectorAll('.frame')];
const creatorButtons = [...document.querySelectorAll('[data-creator]')];
const creators = {
  ziyang: { name: 'Ziyang Tan', role: '设计、架构与开发', credit: 'Design, Architecture & Development' },
  zhuoran: { name: 'Zhuoran Chen', role: '特别致谢 · 讨论与灵感', credit: 'With Special Thanks' },
  sol: { name: 'ChatGPT 5.6 Sol', role: '研究与工程辅助', credit: 'Research & Engineering Assistance' }
};
const positions = { ziyang: 0, zhuoran: 0, sol: 0 };
let currentCreator = 'ziyang';
function showCreator(person, step = 0) {
  if (!Object.hasOwn(creators, person)) return;
  currentCreator = person;
  const group = frames.filter(frame => frame.dataset.person === person);
  positions[person] = (positions[person] + step + group.length) % group.length;
  const selected = group[positions[person]];
  const creator = creators[person];
  folio.dataset.frame = String(frames.indexOf(selected));
  frames.forEach(frame => {
    const active = frame === selected;
    frame.classList.toggle('is-active', active);
    frame.setAttribute('aria-hidden', String(!active));
    frame.inert = !active;
  });
  creatorButtons.forEach(button => {
    if (button.dataset.creator === person) button.setAttribute('aria-current', 'true');
    else button.removeAttribute('aria-current');
  });
  document.querySelector('#creator-name').textContent = creator.name;
  document.querySelector('#creator-role').textContent = creator.role;
  document.querySelector('#creator-credit').textContent = creator.credit;
  const count = document.querySelector('#photo-count');
  count.textContent = `${positions[person] + 1} / ${group.length}`;
  count.setAttribute('aria-label', `当前人物的第 ${positions[person] + 1} 幅画面，共 ${group.length} 幅`);
  const hasMultipleFrames = group.length > 1;
  document.querySelector('#previous').disabled = !hasMultipleFrames;
  document.querySelector('#next').disabled = !hasMultipleFrames;
  document.querySelector('#previous').setAttribute('aria-label', `${creator.name} 的上一张画面`);
  document.querySelector('#next').setAttribute('aria-label', `${creator.name} 的下一张画面`);
  document.querySelector('#frame-status').textContent = `${creator.name}，${creator.role}。画面 ${positions[person] + 1}，共 ${group.length} 幅：${selected.dataset.caption}`;
}
creatorButtons.forEach(button => button.addEventListener('click', () => showCreator(button.dataset.creator)));
document.querySelector('#previous').addEventListener('click', () => showCreator(currentCreator, -1));
document.querySelector('#next').addEventListener('click', () => showCreator(currentCreator, 1));
document.querySelector('#frames').addEventListener('keydown', event => {
  if (event.key !== 'ArrowLeft' && event.key !== 'ArrowRight') return;
  event.preventDefault();
  if (event.target.closest('[data-creator]')) {
    const direction = event.key === 'ArrowRight' ? 1 : -1;
    const currentIndex = creatorButtons.findIndex(button => button.dataset.creator === currentCreator);
    const nextButton = creatorButtons[(currentIndex + direction + creatorButtons.length) % creatorButtons.length];
    showCreator(nextButton.dataset.creator);
    nextButton.focus();
  } else showCreator(currentCreator, event.key === 'ArrowRight' ? 1 : -1);
});
const audio = document.querySelector('#audio');
const playButton = document.querySelector('#play');
const playLabel = document.querySelector('#play-label');
const playIcon = document.querySelector('#play-icon');
const seek = document.querySelector('#seek');
const audioStatus = document.querySelector('#audio-status');
const trackOptions = [...document.querySelectorAll('.track-option')];
let currentTrackLabel = 'Mozart K.576，L1-Clavier';
const formatTime = time => Number.isFinite(time) ? `${Math.floor(time / 60)}:${String(Math.floor(time % 60)).padStart(2, '0')}` : '—:—';
playButton.addEventListener('click', async () => {
  if (audio.paused) {
    try { await audio.play(); }
    catch { audioStatus.textContent = '音频暂时无法播放，请稍后再试。'; }
  } else audio.pause();
});
function syncPlayer() {
  const playing = !audio.paused;
  playLabel.textContent = playing ? '暂停' : '播放';
  playIcon.textContent = playing ? 'Ⅱ' : '▶';
  playButton.setAttribute('aria-label', `${playing ? '暂停' : '播放'} ${currentTrackLabel}`);
}
audio.addEventListener('play', () => { syncPlayer(); });
audio.addEventListener('pause', syncPlayer);
audio.addEventListener('ended', syncPlayer);
audio.addEventListener('loadedmetadata', () => { document.querySelector('#duration').textContent = formatTime(audio.duration); });
audio.addEventListener('timeupdate', () => {
  document.querySelector('#time').textContent = formatTime(audio.currentTime);
  seek.value = Number.isFinite(audio.duration) && audio.duration > 0 ? audio.currentTime / audio.duration * 100 : 0;
  seek.setAttribute('aria-valuetext', `${formatTime(audio.currentTime)} / ${formatTime(audio.duration)}`);
});
seek.addEventListener('input', () => { if (Number.isFinite(audio.duration)) audio.currentTime = Number(seek.value) / 100 * audio.duration; });
audio.addEventListener('error', () => { audioStatus.textContent = '音频暂时无法载入，可在项目档案中聆听。'; });
trackOptions.forEach(option => option.addEventListener('click', () => {
  audio.pause();
  trackOptions.forEach(item => item.setAttribute('aria-pressed', String(item === option)));
  document.querySelector('#track-engine').textContent = option.dataset.engine;
  document.querySelector('#track-composer').textContent = option.dataset.composer;
  document.querySelector('#track-work').textContent = option.dataset.work;
  document.querySelector('#track-name').textContent = option.dataset.name;
  document.querySelector('#track-mode').textContent = option.dataset.mode;
  currentTrackLabel = `${option.dataset.composer} ${option.dataset.work}，${option.dataset.mode}`;
  audio.src = option.dataset.src;
  audio.load();
  seek.value = 0;
  document.querySelector('#time').textContent = '0:00';
  document.querySelector('#duration').textContent = '—:—';
  audioStatus.textContent = option.dataset.engine.replace(/^\d+ \/ /, '');
  syncPlayer();
}));
const panels = [...document.querySelectorAll('dialog.panel')];
document.querySelectorAll('[data-open]').forEach(link => {
  link.addEventListener('click', event => {
    event.preventDefault();
    const panel = document.getElementById(link.dataset.open);
    panel.showModal();
    document.body.style.overflow = 'hidden';
  });
});
panels.forEach(panel => {
  panel.querySelector('[data-close]').addEventListener('click', () => panel.close());
  panel.addEventListener('close', () => {
    document.body.style.overflow = '';
    if (panel.id === 'sound') audio.pause();
  });
});

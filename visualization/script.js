const nodes = new Map();
const edges = new Map();
const edgeArr = [];
const stops = [];
const trafficLights = [];
let physicalNetworkReady = false;
let resultsReady = false;

let trams = [];

let markedNodes = [];

let lastX = -1;
let lastY = -1;

let camY = 0;
let camX = 0;
let zoom = -4;
let zoomVal = 2 ** zoom;
let wayStep = Math.ceil(400 / zoomVal);

let time_rate = 10;
let time = 18000;
let lastT;
let paused = false;

document.addEventListener('mousedown', e => {
  lastX = e.x;
  lastY = e.y;
});

document.addEventListener('mousemove', e => {
  if (e.buttons == 0)
    return;

  if (lastX != -1) {
    camX -= (e.x - lastX) / zoomVal;
    camY += (e.y - lastY) / zoomVal;
  }

  lastX = e.x;
  lastY = e.y;
});

document.addEventListener('wheel', e => {
  zoom = zoom - e.deltaY / 1000;
  zoomVal = 2 ** zoom;

  wayStep = Math.ceil(0.5 / zoomVal);
});

document.addEventListener("mousedown", e => {
  if (e.button == 2) {
    const x = (e.clientX - windowWidth / 2) / zoomVal + camX;
    const y = (-e.clientY + windowHeight / 2) / zoomVal + camY;

    const ent = find(x, y);
    console.log(ent);
    if (ent.hasOwnProperty("t0"))
      tmark(ent.id);
    else
      mark(ent.id);
  }
});

document.addEventListener("keydown", event => {
  if (event.isComposing || event.keyCode === 32) {
    paused = paused ? false : true;
  }
});

function setup() {
  createCanvas(windowWidth, windowHeight);

  fetchNetwork().then(fetchResult);

  lastT = new Date().getTime();
  textSize(30);
}

function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
}

///////////////////////////////////////////////////////////////////////////////////////////

function draw() {
  background(255);
  fill(0, 0, 0, 0);

  const newT = new Date().getTime();
  const simulation_dt = time_rate * (newT - lastT) / 1000;
  lastT = newT;

  if (!paused)
    time = time + simulation_dt;

  if (!physicalNetworkReady)
    return;

  //stroke(0, 0, 0, 100);
  stroke(0, 0, 0, 255);
  strokeWeight(3);
  for (let [id, edge] of edges) {
    drawWay(edge);
  }

  if (resultsReady) {
    strokeWeight(3);
    fill(0, 0, 0, 0);
    for (let tl of trafficLights) {
      drawTrafficLight(tl);
    }

    strokeWeight(3);
    stroke(0);
    for (let tram of trams) {
      drawTram(tram);
    }
  }

  strokeWeight(7);
  stroke(0, 255, 255, 255);
  for (let s of stops) {
    drawNode(s);
  }

  stroke(255, 0, 255);
  strokeWeight(5);
  fill(0, 0, 0, 0);
  for (let n of markedNodes) {
    const p = tr(n.x, n.y);
    circle(p[0], p[1], 30);
  }

  fill(255);
  stroke(0);
  strokeWeight(3);

  const h = Math.floor(time / 3600);
  let m = Math.floor((time - 3600 * h) / 60);
  let s = Math.floor(time - 3600 * h - 60 * m);
  m = m.toString();
  s = s.toString();
  if (m.length == 1) m = "0" + m;
  if (s.length == 1) s = "0" + s;

  text(h + ":" + m + ":" + s, 10, 40);

  //text(Math.floor(time), 10, 40);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

function tr(x, y) { //transform point
  const pos = [];
  pos[0] = (x - camX) * zoomVal + windowWidth / 2;
  pos[1] = (camY - y) * zoomVal + windowHeight / 2;
  return pos;
}

function drawNode(node) {
  const p = tr(node.x, node.y);
  point(p[0], p[1]);
}

function drawWay(way) {
  const n0 = way.nodes[0];
  lastP = tr(n0.x, n0.y);

  for (let i = 1; i < way.nodes.length - 1; i += wayStep) {
    //for (let i = 1; i < way.nodes.length - 1; i++) {
    n = way.nodes[i];
    p = tr(n.x, n.y);

    line(lastP[0], lastP[1], p[0], p[1]);
    lastP = p;
  }
  const lastLastNode = way.nodes[way.nodes.length - 1];
  const lastLastP = tr(lastLastNode.x, lastLastNode.y);
  line(lastP[0], lastP[1], lastLastP[0], lastLastP[1]);
}

function drawTrafficLight(node) {
  let i = 0;
  while (node.time[i + 1] < time)
    ++i;

  const state = node.state[i];


  if (state == 1) {
    stroke(0, 255, 0);
  }
  else {
    stroke(255, 0, 0);
  }
  const p = tr(node.x, node.y);
  circle(p[0], p[1], 15);
}

function mark(id) { //mark node
  const n = nodes.get(id);
  if (markedNodes.includes(n)) {
    return unmark(n.id);
  }

  markedNodes.push(n);
  return n;
}

function unmark(id) { //unmark node
  if (id == undefined) {
    routes.length = 0;
    markedNodes = [];
    return;
  }

  for (let i = markedNodes.length - 1; i != -1; i--) {
    const n = markedNodes[i];
    if (n.id == id) {
      markedNodes.splice(i, 1);
      return n;
    }
  }
}

function tmark(tid) { //mark tram
  const tram = trams.filter(x => x.id == tid)[0];
  if (tram.marked)
    tram.marked = false;
  else
    tram.marked = true;

  return tram;
}

function find(x, y) { //find entity
  let foundE;
  let foundDis = Number.POSITIVE_INFINITY;
  for (let [ix, n] of nodes) {
    const dis = (x - n.x) ** 2 + (y - n.y) ** 2
    if (dis < foundDis) {
      foundDis = dis;
      foundE = n;
    }
  }

  for (let tram of trams) {
    const [tx, ty] = tramPosition(tram);
    const dis = (x - tx) ** 2 + (y - ty) ** 2
    if (dis < foundDis) {
      foundDis = dis;
      foundE = tram;
    }
  }
  return foundE;
}

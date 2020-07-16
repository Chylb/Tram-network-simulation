const progressBar = document.getElementById("progressBar");

async function downloadWithProgress(url) {
  const response = await fetch(url);
  const reader = response.body.getReader();
  const contentLength = response.headers.get('Content-Length');

  let receivedLength = 0;
  let chunks = [];
  while (true) {
    const { done, value } = await reader.read();

    if (done) {
      break;
    }

    chunks.push(value);
    receivedLength += value.length;

    progressBar.innerHTML = "Loading... " + (100 * receivedLength / contentLength).toFixed(0) + "%";
  }

  let chunksAll = new Uint8Array(receivedLength);
  let position = 0;
  for (let chunk of chunks) {
    chunksAll.set(chunk, position);
    position += chunk.length;
  }

  let result = new TextDecoder("utf-8").decode(chunksAll);
  let commits = JSON.parse(result);

  return commits;
}

async function fetchNetwork() {
  const network_model = await (await fetch('./network_visualization_model.json')).json();

  for (let n of network_model.nodes) {
    n.accessibleNodes = [];
    if (nodes.has(n.id))
      console.log("NODE ERR");
    nodes.set(n.id, n);

    if (n.hasOwnProperty("trafficLight")) {
      trafficLights.push(n);
    }

    if (n.hasOwnProperty("stopName")) {
      let routeNodeStops = routeNodeStopsMap.get(n.stopName);
      if (routeNodeStops == undefined) {
        routeNodeStopsMap.set(n.stopName, [])
        routeNodeStops = routeNodeStopsMap.get(n.stopName);
      }

      routeNodeStops.push(n);
    }
  }

  for (let e of network_model.edges) {
    edgeArr.push(e);
    if (edges.has(e.id)) {
      console.log("EDGE ERR");
    }

    e.nodes = e.nodes.map(id => nodes.get(id));
    e.lengths = [];
    e.cumulativeLengths = [0];

    let cumulativeLength = 0;
    let prevNode = e.nodes[0];
    for (let i = 1; i < e.nodes.length; i++) {
      const node = e.nodes[i];
      const length = Math.sqrt((node.x - prevNode.x) ** 2 + (node.y - prevNode.y) ** 2);
      cumulativeLength += length;

      e.lengths.push(length);
      e.cumulativeLengths.push(cumulativeLength);

      prevNode = node;
    }

    e.cumulativeLengths.pop();

    edges.set(e.id, e);
  }

  for (let e of edgeArr) {
    for (let i = 0; i < e.nodes.length - 1; i++) {
      const node = e.nodes[i].accessibleNodes.push(e.nodes[i + 1]);
    }
  }

  console.log(network_model);
  console.log("NETWORK LOADED");

  physicalNetworkReady = true;
}

async function fetchResult() {
  const results = await downloadWithProgress("./results.json");

  for (let tram of results.trams) {

    tram.n = tram.time.length;
    if (tram.n == 0)
      continue;
    tram.t0 = tram.time[0];
    tram.t1 = tram.time[tram.n - 1];

    tram.marked = false;
    trams.push(tram);

    for (let i = 0; i < tram.n; i++) {
      tram.edge[i] = edges.get(tram.edge[i]);
    }

    const keys = [];
    const rows = [];
    for (let i = 0; i < tram.n; i++) {
      const row = {
        time: tram.time[i],
        state: tram.state[i],
        position: tram.position[i],
        speed: tram.speed[i],
        edge: tram.edge[i],
      }
      keys.push(tram.time[i]);
      rows.push(row);
    }
    tram.rowsTree = new RangeTree(keys, rows);
    tram.rows = rows;

    const passengerKeys = [];
    const passengerRows = [];
    for (let i = 0; i < tram.passengerTime.length; i++) {
      const passengerRow = {
        time: tram.passengerTime[i],
        passengers: tram.passengers[i]
      }
      passengerKeys.push(passengerRow.time);
      passengerRows.push(passengerRow);
    }
    tram.passengerRowsTree = new RangeTree(passengerKeys, passengerRows);
    tram.passengerRows = passengerRows;
  }

  for (let trafficLight of results.trafficLights) {
    const node = nodes.get(trafficLight.id);
    node.time = trafficLight.time;
    node.state = trafficLight.state;
  }

  for (let routeNode of results.routeNodes) {
    const keys = [];
    const rows = [];
    for (let i = 0; i < routeNode.time.length; i++) {
      const row = {
        time: routeNode.time[i],
        passengers: routeNode.passenger[i]
      }
      keys.push(row.time);
      rows.push(row);
    }
    const routeNodeTree = new RangeTree(keys, rows);

    for (let n of routeNodeStopsMap.get(routeNode.name)) {
      n.rows = rows;
      n.rowsTree = routeNodeTree;
      tramStops.push(n);
    }
  }

  resultsReady = true;
  console.log(results);
  console.log("SIMULATION RESULTS LOADED");

  progressBar.style.display = "none";
}

const timeSlider = document.getElementById("timeSlider");
timeSlider.oninput = e => {
  time = new Number(timeSlider.value);
}

const timeRateValue = document.getElementById("timeRateValue");
const timeRateSlider = document.getElementById("timeRateSlider");
timeRateSlider.oninput = e => {
  const val = new Number(timeRateSlider.value);
  time_rate = (Math.exp(val)) - 1;
  timeRateValue.innerHTML = "Time rate " + time_rate.toFixed(1);
}

timeSlider.oninput();
timeRateSlider.oninput();
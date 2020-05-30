function accXofT(t, v0) {
  return v0 * t + 0.5 * 1.4 * t ** 2;
}

function decXofT(t, v0) {
  return v0 * t - 0.5 * 1.4 * t ** 2;
}

function drawTram(tram) {
  if (time < tram.t0 || tram.t1 < time)
    return;

  let i = 0;
  while (tram.time[i + 1] < time)
    ++i;

  const edge = tram.edge[i];

  let position = tram.position[i];
  const state = tram.state[i];
  const v0 = tram.speed[i];

  const dt = time - tram.time[i];

  if (state == 1) {
    fill(0, 0, 255);
    position += accXofT(dt, v0);
  }
  else if (state == 2) {
    fill(255, 0, 0);
    position += decXofT(dt, v0);
  }
  else if (state == 3) {
    fill(255, 0, 255);
    position += v0 * dt;
  }
  else {
    fill(0, 0, 0);
  }

  let j = 1;
  while (position > edge.cumulativeLengths[j]) {
    j++;
  }

  const head = edge.nodes[j];
  const tail = edge.nodes[j - 1];

  const unitVectorX = (head.x - tail.x) / edge.lengths[j - 1];
  const unitVectorY = (head.y - tail.y) / edge.lengths[j - 1];

  position -= edge.cumulativeLengths[j - 1];
  const x = tail.x + unitVectorX * position;
  const y = tail.y + unitVectorY * position;

  const p = tr(x, y);
  circle(p[0], p[1], 11)

  if (tram.marked ) {
    stroke(0, 0, 255);
    fill(0, 0, 0, 0);
    circle(p[0], p[1], 25);
    stroke(0);
  }

}

function tramPosition(tram) {
  if (time < tram.t0 || tram.t1 < time)
    return [NaN, NaN];

  let i = 0;
  while (tram.time[i + 1] < time)
    ++i;

  const edge = tram.edge[i];

  let position = tram.position[i];
  const state = tram.state[i];
  const v0 = tram.speed[i];

  const dt = time - tram.time[i];

  if (state == 1) {
    position += accXofT(dt, v0);
  }
  else if (state == 2) {
    position += decXofT(dt, v0);
  }
  else if (state == 3) {
    position += v0 * dt;
  }

  let j = 1;
  while (position > edge.cumulativeLengths[j]) {
    j++;
  }
  const head = edge.nodes[j];
  const tail = edge.nodes[j - 1];

  const unitVectorX = (head.x - tail.x) / edge.lengths[j - 1];
  const unitVectorY = (head.y - tail.y) / edge.lengths[j - 1];

  position -= edge.cumulativeLengths[j - 1];
  const x = tail.x + unitVectorX * position;
  const y = tail.y + unitVectorY * position;

  return [x, y];
}

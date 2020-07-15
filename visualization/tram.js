function accXofT(t, v0) {
  return v0 * t + 0.5 * 1.4 * t ** 2;
}

function decXofT(t, v0) {
  return v0 * t - 0.5 * 1.4 * t ** 2;
}

function drawTram(tram) {
  if (time < tram.t0 || tram.t1 < time)
    return;

  const row = tram.rowsTree.find(time);
  const edge = row.edge;
  let position = row.position;
  const state = row.state;
  const v0 = row.speed;
  const dt = time - row.time;

  const passenerRow = tram.passengerRowsTree.find(time);

  fill(255 / 310 * passenerRow.passengers);

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

  const p = tr(x, y);
  circle(p[0], p[1], 15)

  if (tram.marked) {
    stroke(0, 0, 255);
    strokeWeight(4);
    fill(0, 0, 0, 0);
    circle(p[0], p[1], 30);

    stroke(0);
    strokeWeight(3);
  }
}

function tramPosition(tram) {
  if (time < tram.t0 || tram.t1 < time)
    return [NaN, NaN];

  const row = tram.rowsTree.find(time);
  const edge = row.edge;
  let position = row.position;
  const state = row.state;
  const v0 = row.speed;
  const dt = time - row.time;

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

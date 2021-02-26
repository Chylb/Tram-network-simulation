module.exports = {
	createRouteNodes: function (pn, ln) {

		for (let route of ln.routes) {
			for (let i = 0; i < route.stops.length - 1; ++i) {
				const stopName = pn.nodes.get(route.stops[i]).tags.name;
				const nextStopName = pn.nodes.get(route.stops[i + 1]).tags.name;

				let routeEdge = ln.routeEdges.get(stopName + "_" + nextStopName);
				if (routeEdge == undefined) {
					routeEdge = {
						head: nextStopName,
						tail: stopName,
						lines: []
					};

					ln.routeEdges.set(stopName + "_" + nextStopName, routeEdge);
				}

				routeEdge.lines.push(route.id);

				if (ln.routeNodes.get(stopName) == undefined) {
					const stops = pn.stopsIds.get(stopName).map(x => pn.nodes.get(x));

					const routeNode = {
						name: stopName,
						x: stops.reduce((a, b) => a + b.x, 0) / stops.length,
						y: stops.reduce((a, b) => a + b.y, 0) / stops.length
					};
					ln.routeNodes.set(stopName, routeNode);
				}
			}
		}
	},

	setRouteNodesProperties: function (ln) {
		const residential = generatePassengerProperties(
			[0, 100, 400, 120, 400, 100, 0],
			[0, 4, 7, 9, 16, 18, 23],
			[0, 25, 100, 120, 100, 100, 0],
			[0, 4, 7, 9, 16, 18, 23],
		);

		const central = generatePassengerProperties(
			[0, 25, 100, 120, 100, 100, 0],
			[0, 4, 7, 9, 16, 18, 23],
			[0, 100, 400, 120, 400, 100, 0],
			[0, 4, 7, 9, 16, 18, 23],
		);

		const highInterest = generatePassengerProperties(
			[0, 125, 500, 600, 500, 500, 0],
			[0, 4, 7, 9, 16, 18, 23],
			[0, 500, 2000, 600, 2000, 500, 0],
			[0, 4, 7, 9, 16, 18, 23],
		);

		for (let node of ln.routeNodes.values()) {
			node.properties = residential;
		}

		const cityCentre = getRouteNodesInsideArea(ln, [[-1290, 1140], [140, 1620], [1330, 1250], [1630, 470], [1900, -300], [300, -3700], [-700, -3500]]);
		for (let node of cityCentre)
			node.properties = central;

		const highInterestNodes = ["Norymberska", "Ruczaj", "Chmieleniec", "Politechnika", "Rondo Mogilskie", "Czerwone Maki P+R" , "Muzeum Narodowe", "Rondo Czyżyńskie", "Kampus UJ"].map(x => ln.routeNodes.get(x));
		for (let node of highInterestNodes)
			node.properties = highInterest;


		for (let h = 0; h < 24; ++h) {
			let generationSum = 0;
			let absorptionSum = 0;

			for (let node of ln.routeNodes.values()) {
				generationSum += node.properties.generationRate[h];
				absorptionSum += node.properties.absorptionRate[h];
			}

			if (generationSum != absorptionSum) {
				console.log("Passenger generation sum %f is different than absorption sum %f at %d o'clock by %f%", generationSum, absorptionSum, h,
					(100 * (Math.max(generationSum, absorptionSum) / Math.min(generationSum, absorptionSum) - 1)).toFixed(2)
				);
			}
		}
	}
}

function generatePassengerProperties(generationRate, generationTime, absorptionRate, absorptionTime) {
	const properties = {
		generationRate: [],
		generationDistribution: [],
		absorptionRate: [],
		expectedGeneratedCount: 0
	};

	let j = 0;
	let rate = generationRate[0];
	let sum = 0;

	for (let h = 0; h < 24; h++) {
		if (h >= generationTime[j + 1]) {
			j++;
			rate = generationRate[j];
		}

		sum += rate;
		properties.generationDistribution[h] = sum;
		properties.generationRate[h] = rate;
	}
	properties.expectedGeneratedCount = sum;

	j = 0;
	rate = absorptionRate[j];
	for (let h = 0; h < 24; h++) {
		if (h >= absorptionTime[j + 1]) {
			j++;
			rate = absorptionRate[j];
		}
		properties.absorptionRate[h] = rate;
	}

	return properties;
}

function getRouteNodesInsideArea(ln, area) {
	const nodesInside = [];
	for (let node of ln.routeNodes.values())
		if (inside([node.x, node.y], area))
			nodesInside.push(node)

	return nodesInside;

	function inside(point, vs) {
		//code from https://github.com/substack/point-in-polygon

		let x = point[0], y = point[1];

		let inside = false;
		for (let i = 0, j = vs.length - 1; i < vs.length; j = i++) {
			let xi = vs[i][0], yi = vs[i][1];
			let xj = vs[j][0], yj = vs[j][1];

			let intersect = ((yi > y) != (yj > y))
				&& (x < (xj - xi) * (y - yi) / (yj - yi) + xi);
			if (intersect) inside = !inside;
		}

		return inside;
	};
}
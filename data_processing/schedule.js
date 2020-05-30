const findPath = require("./graph.js").findPath;

module.exports = {
    createRoutes: function (physicalNetwork, schedule, logicalData) {
        routeStart = new Map();
        routeEnd = new Map();

        routeStart.set("Wzgórza Krzesławickie - Salwator", 213578451);
        routeEnd.set("Wzgórza Krzesławickie - Salwator", 2419732952);
        routeStart.set("Salwator - Wzgórza Krzesławickie", 2419732952);
        routeEnd.set("Salwator - Wzgórza Krzesławickie", 2426087293);

        routeStart.set("Nowy Bieżanów P+R - Krowodrza Górka", 287586332);
        routeEnd.set("Nowy Bieżanów P+R - Krowodrza Górka", 257656078);
        routeStart.set("Krowodrza Górka - Nowy Bieżanów P+R", 257655889);
        routeEnd.set("Krowodrza Górka - Nowy Bieżanów P+R", 623562103);

        let id = 0;

        for (let line of schedule.lines) {
            dir1Start = routeStart.get(line.direction1.name);
            dir1End = routeEnd.get(line.direction1.name);

            dir2Start = routeStart.get(line.direction2.name);
            dir2End = routeEnd.get(line.direction2.name);

            let route1;
            let route2;

            if (dir1Start != undefined) {
                route1 = processRoute(physicalNetwork, line.direction1, dir1Start, dir1End);
                route1.id = id;
                id++;
            } 
            if (dir2Start != undefined) {
                route2 = processRoute(physicalNetwork, line.direction2, dir2Start, dir2End);
                route2.id = id;
                id++;
            }

            if (route1 != undefined)
                logicalData.routes.push(route1);

            if (route2 != undefined)
                logicalData.routes.push(route2);
        }
    },

    createTrips: function (logicalData) {
        for (let route of logicalData.routes) {
            const times = [];

            for (let i = 0; i < route.scheduleRoute.stops.length; i++) {
                const stop = route.scheduleRoute.stops[i];
                times[i] = [];

                for (let j = 0; j < stop.schedule.length; j++) {
                    let h = parseInt(stop.schedule[j].Godzina);
                    if (isNaN(h))
                        continue;

                    if (h < 2)
                        h += 24;

                    const ms = stop.schedule[j]["Dzień powszedni"].split(" ");

                    for (let m of ms) {
                        const mn = parseInt(m);
                        if (isNaN(mn))
                            continue;

                        times[i].push(h * 60 + mn);
                    }
                }
            }

            let tripCount = times[0].length;
            for (let row of times)
                if (row.length > tripCount)
                    tripCount = row.length;
            const stopCount = times.length;

            for (let i = 0; i < tripCount; i++) {
                const trip = {
                    route: route.id,
                    times: [],
                    start: 0,
                    end: 0,
                    duration: 0
                };

                let lastStopTime = -1;

                for (let j = 0; j < stopCount; j++) {
                    let k = 0;

                    while (k < times[j].length) {
                        if (times[j][k] > lastStopTime && (lastStopTime == -1 || times[j][k] - lastStopTime < 8))
                            break;
                        k++;
                    }

                    if (k < times[j].length) {
                        trip.times[j] = times[j][k];
                        lastStopTime = trip.times[j];
                    }
                    else
                        trip.times[j] = -1;

                    times[j][k] = -1;
                }

                let j = 0;
                while (trip.times[j] == -1)
                    j++;

                trip.start = trip.times[j];
                trip.end = lastStopTime;
                trip.duration = trip.end - trip.start;

                if(trip.times.includes(-1))
                    continue;

                logicalData.trips.push(trip);
            }

            let error = false;
            for (let t of times) {
                for (let r of t) {
                    if (r != -1)
                        error = true;
                }
            }

            if (error) {
                console.log("TRIP ERROR ");
                console.log(route);
            }
        }
    }
};

function processRoute(physicalNetwork, scheduleRoute, start, end) { //this function finds route's path
    const route = {
        nodes: [], //list of nodes that the route goes through
        stops: [], //stop nodes
        scheduleRoute: scheduleRoute //reference to route data in schedule
    };

    const initialNode = physicalNetwork.nodes.get(start);
    //console.log(initialNode);
    route.nodes.push(initialNode.id);
    //route.nodes.push(start);

    let currStop = initialNode;
    route.stops.push(initialNode.id);

    for (let i = 1; i < scheduleRoute.stops.length - 1; i++) {
        const nextStop = scheduleRoute.stops[i].name;
        const next2Stop = scheduleRoute.stops[i + 1].name;

        let path;
        const target1 = [...physicalNetwork.stopsIds.get(nextStop)];
        //console.log(target1);
        while (target1.length != 0) {
            const [dis1, path1] = findPath(currStop, target1);
            const intermediatePlatform = physicalNetwork.nodes.get(path1[0]);
            //console.log(next2Stop);
            //console.log(physicalNetwork.stopsIds.get(next2Stop));
            const [dis2, path2] = findPath(intermediatePlatform, physicalNetwork.stopsIds.get(next2Stop));

            //console.log(intermediatePlatform);

            const endPlatform = physicalNetwork.nodes.get(path2[0]);
            const physicalDistance = Math.sqrt((endPlatform.x - intermediatePlatform.x) ** 2 + (endPlatform.y - intermediatePlatform.y) ** 2);

            //console.log(endPlatform);

            if (dis2 < 2 * physicalDistance) {
                path = path1;
                break;
            }
            else {
                for (let i = target1.length - 1; i != -1; i--) {
                    const n = target1[i];
                    if (n == path1[0])
                        target1.splice(i, 1);
                }
                //console.log(target1);
            }
        }
        route.nodes = path.concat(route.nodes);
        path.push(currStop.id);
        currStop = physicalNetwork.nodes.get(path[0]);

        route.stops.push(currStop.id);
    }

    const [dis, lastPath] = findPath(currStop, [end]);
    route.nodes = lastPath.concat(route.nodes);
    route.nodes.reverse();
    route.stops.push(end);

    return route;
}
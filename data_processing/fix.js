module.exports = {
    fixStopsNames: function (pn) {
        const fixedNames = new Map();
        fixedNames.set("Tauron Arena Kraków Al. Pokoju n/ż", "TAURON Arena Kraków Al. Pokoju");
        fixedNames.set("Św. Wawrzyńca", "Św.Wawrzyńca");
        fixedNames.set("Elektromontaż N/Ż", "Elektromontaż");
        fixedNames.set("Plac Centralny im. R. Reagana", "Plac Centralny im. R.Reagana");
        fixedNames.set("Os. Kolorowe", "Os.Kolorowe");
        fixedNames.set("Os. Zgody", "Os.Zgody");
        fixedNames.set("Kopiec Wandy NŻ", "Kopiec Wandy");

        for (let stop of pn.stops) {
            const fixedName = fixedNames.get(stop.tags.name);
            if (fixedName != undefined)
                stop.tags.name = fixedName;

        }
    },

    fixMissingStops: function (pn, stopsTmp) { //fixes missing stops platforms
        const czyżynyStop1 = stopsTmp.filter(x => x.id == 2163355803)[0];
        const missingPlatform = pn.nodes.get(2163355804);
        missingPlatform.tags = czyżynyStop1.tags;
        stopsTmp.push(missingPlatform);
    },

    removeBannedNodes: function (pn) { //removes nodes that are not needed or creates problems
        const bannedNodes = [];
        bannedNodes.push(2431249451, 1578667749, 1578667767, 2375524704); //removing nodes from Podgórze depot
        bannedNodes.push(2756848363, 1770194425, 2163355842, 2163355843, 3161355031, 3161187684, 3048453154); //removing nodes from Nowa Huta depot

        for (let bannedNodeId of bannedNodes) {
            removeNode(pn, bannedNodeId);
        }
    },

    removeFloatingIslands: function (pn) { //removes tracks not connected to the rest of network
        const floatingIslandsMainNodes = [];
        floatingIslandsMainNodes.push(3115160964, 2375524480, 3115162080); //removing Podgórze depot island
        floatingIslandsMainNodes.push(2756848446, 3161201333, 3043224876, 3045264186, 3162542065, 3040772123); //removing Nowa huta depot island

        const floatingIslandsNodes = [];

        for (let islandMainNode of floatingIslandsMainNodes) {
            const visited = [];
            const toVisit = [pn.nodes.get(islandMainNode)];

            let node;
            while (toVisit.length > 0) {
                node = toVisit[0];

                for (let n of node.adjacentNodes) {
                    if (!visited.includes(n) && !toVisit.includes(n)) {
                        toVisit.push(n);
                    }
                }
                toVisit.shift();
                visited.push(node);
            }

            floatingIslandsNodes.push(...visited);
        }

        for (let node of floatingIslandsNodes) {
            removeNode(pn, node.id);
        }
    },

    fixWaysDirections: function (pn) {
        for (let track of pn.tracks)
            if (!track.tags.hasOwnProperty("oneway"))
                track.tags.oneway = "no";

        const wrongWays = pn.tracks.filter(x => x.id == 603467296);
        wrongWays[0].tags.oneway = "yes";
    },

    fixMissingMaxspeed: function (pn) {
        for (let track of pn.tracks)
            if (!track.tags.hasOwnProperty("maxspeed"))
                track.tags.maxspeed = 50;
            else
                track.tags.maxspeed = parseInt(track.tags.maxspeed);
    },

    //////////////////////////////////////////////////////////////////

    removeFakeRouteStops: function (schedule) {
        schedule.lines[5].direction1.stops.splice(3, 1); //there is no such a stop in this direction
        schedule.lines[11].direction1.stops.splice(3, 1);
        schedule.lines[13].direction1.stops.splice(3, 1);
    }
};

function removeNode(pn, bannedNodeId) {
    for (let i = pn.tracks.length - 1; i >= 0; i--) { //removing tracks that include banned nodes
        const t = pn.tracks[i];
        if (t.nodes.includes(bannedNodeId)) {
            pn.tracks.splice(i, 1);
        }
    }
    pn.nodes.delete(bannedNodeId);
}
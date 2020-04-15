module.exports = {
    fixStopsNames: function (data) {
        const fixedNames = new Map();
        fixedNames.set("Tauron Arena Kraków Al. Pokoju n/ż", "TAURON Arena Kraków Al. Pokoju");
        fixedNames.set("Św. Wawrzyńca", "Św.Wawrzyńca");
        fixedNames.set("Elektromontaż N/Ż", "Elektromontaż");
        fixedNames.set("Plac Centralny im. R. Reagana", "Plac Centralny im. R.Reagana");
        fixedNames.set("Os. Kolorowe", "Os.Kolorowe");
        fixedNames.set("Os. Zgody", "Os.Zgody");
        fixedNames.set("Kopiec Wandy NŻ", "Kopiec Wandy");

        for(let stop of data.stops) {
            const fixedName = fixedNames.get(stop.tags.name);
            if(fixedName != undefined) 
                stop.tags.name= fixedName;
            
        }
    },

    fixMissingStops: function (data, stopsTmp) { //fixes missing stops platforms
        const czyżynyStop1 = stopsTmp.filter(x => x.id == 2163355803)[0];
        const missingPlatform = data.nodes.get(2163355804);
        missingPlatform.tags = czyżynyStop1.tags;
        stopsTmp.push(missingPlatform);
    },

    removeBannedNodes: function (data) {
        const bannedNodes = [3114830183, 213575668, 2163355826, 1571736117]; //they are banned because they cause problems in finding path of line routes //POPRAW ##############################🛠️🛠️🛠️🛠️

        for (let bannedNodeId of bannedNodes) {
            for (let i = data.tracks.length - 1; i >= 0; i--) { //removing tracks that include banned nodes
                const t = data.tracks[i];
                if (t.nodes.includes(bannedNodeId))
                    data.tracks.splice(i, 1);
            }
            data.nodes.delete(bannedNodeId);
        }
    },

    fixWaysDirections: function (data) {
        for (let track of data.tracks)
            if (!track.tags.hasOwnProperty("oneway"))
                track.tags.oneway = "no";

        const wrongWays = data.tracks.filter(x => x.id == 603467296);
        wrongWays[0].tags.oneway = "yes";
    },

    fixMissingMaxspeed: function (data) {
        for (let track of data.tracks)
            if (!track.tags.hasOwnProperty("maxspeed"))
                track.tags.maxspeed = 50;
            else
                track.tags.maxspeed = parseInt(track.tags.maxspeed);
    },

    //////////////////////////////////////////////////////////////////

    removeFakeRouteStops: function (schedule) {
        schedule.lines[5].direction1.stops.splice(3, 1); //there is no such a stop in this direction
        schedule.lines[11].direction1.stops.splice(3, 1);
        schedule.lines[14].direction1.stops.splice(3, 1);
    }
};
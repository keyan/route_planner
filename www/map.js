var map;
var source;
var target;
var line;

$(document).ready(function(){
    // Create the map centered in Seattle, restrict bounds to area used
    // for graph construction.
    map = L.map('map_canvas').setView([47.607413, -122.323370], 13);
    var southWest = L.latLng(47.5565, -122.3692);
    var northEast = L.latLng(47.6503, -122.2716);
    map.setMaxBounds(L.latLngBounds(southWest, northEast));

    L.tileLayer('https://api.tiles.mapbox.com/v4/{id}/{z}/{x}/{y}.png?access_token={accessToken}', {
    attribution: 'Map data &copy; <a href="https://www.openstreetmap.org/">OpenStreetMap</a> contributors, <a href="https://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>, Imagery © <a href="https://www.mapbox.com/">Mapbox</a>',
    maxZoom: 18,
    id: 'mapbox.streets',
    accessToken: 'pk.eyJ1Ijoia3Bpc2hkYWRpYW4iLCJhIjoiY2pvMTVzN2lwMDdrbTNrcG9mb2RvcDF5aSJ9.wbFcMArvH5wSmVwvRomNmg'
}).addTo(map);

    source = L.marker([47.620959, -122.350666], {draggable: true}).addTo(map);
    target = L.marker([47.598289, -122.334700], {draggable: true}).addTo(map);
    line = L.polyline([], {color: 'red'}).addTo(map);
    routeAndRedraw();

    source.on('dragend', routeAndRedraw);
    target.on('dragend', routeAndRedraw);
});

function routeAndRedraw() {
    var url = "http://127.0.0.1:4001/" +
        source.getLatLng().lat + "," + source.getLatLng().lng + "," +
        target.getLatLng().lat + "," + target.getLatLng().lng;
    $.ajax(url, { dataType: "jsonp" });
}

function routingEngineCallback(json) {
    line.remove();
    // See github.com/jieter/Leaflet.encoded
    line = L.polyline(L.PolylineUtil.decode(json.polyline, {precision: 6}), {color: 'red'}).addTo(map);

    target.unbindPopup();
    target
        .bindPopup(`Time to destination: ${json.travel_time}`)
        .openPopup();
}

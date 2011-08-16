Drupal.behaviors.simplestGmap = function (context) {
  if (!GBrowserIsCompatible()) {
    return;
  }
  
  $.each(Drupal.settings.simplest_gmap || {}, function(map_id, map) {
    sgInitMap(map);
  });   
}



function sgInitMap(map) {  
  window.simplest_gmaps = window.simplest_gmaps || {};
  
  if (!document.getElementById(map.map_id)) return;
  
  var gmap = new GMap2(document.getElementById(map.map_id));  
  
  gmap.map = map;  
  
  window.simplest_gmaps[map.map_id] = gmap;
  
  map.latitude = (map.latitude == null) ? '' : map.latitude;
  map.longitude = (map.longitude == null) ? '' : map.longitude;
  
  var marker_exists = (map.latitude + '').length && (map.longitude + '').length;
  
  if (map.input_mode) {
    sgHideInputs(map.elements_name);
  }
  
  gmap.setMapType(window[map.defaults.map_type]);
  if (map.defaults.control_overview) {
    gmap.addControl(new GOverviewMapControl());
  }
  
  if (map.defaults.control_scale) {
    gmap.addControl(new GScaleControl());
  }
  
  if (map.defaults.control_size) {    
    gmap.addControl(new window[map.defaults.control_size]());
  }
  
  if (marker_exists) {
  // we have marker, so center map to this marker and add controls to remove it
    map.zoom = parseInt(map.zoom);    
    
    gmap.setCenter(new GLatLng(map.latitude, map.longitude), map.zoom);
    
    sgAddMarker(gmap, gmap.getCenter());   
  // if no marker, init map with default settings and add 'add marker' controls
  } else {
    gmap.setCenter(new GLatLng(map.defaults.map_latitude, map.defaults.map_longitude), parseInt(map.defaults.map_zoom));    
  }
  
   $('#' + map.map_id).after('<div id="' + map.map_id + '_controls' + '"></div>');
   sgUpdateControls(gmap);
  
   if (!map.defaults.control_nozoom || map.input_mode) {
     gmap.enableScrollWheelZoom();    
   }
   gmap.enableContinuousZoom();
   
   if (map.defaults.control_nodrag && !map.input_mode) {
     gmap.disableDragging();
   }
   gmap.disableDoubleClickZoom();
     
  
  // if it is an admin mode, where moving map should update inputs (that are saved to Drupal db) set listeners for updating
  if (map.input_mode) {
    GEvent.addListener(gmap, "zoomend", function() {
      sgUpdateInputs(gmap);
    });  
    
    GEvent.addListener(gmap, "dblclick", function(overlay, latlng) {
      if (gmap.map.marker) {
        sgDeleteMarker(gmap);
      }
      
      sgAddMarker(gmap, latlng);
    });    
  }
  
}

function sgUpdateInputs(gmap) {  
  // if we are not in admin mode 
  // we don't need to pass map data to Drupal inputs.   
  if (!gmap.map.input_mode) return;
  
  if (gmap.map.marker) {    
    var coords = gmap.map.marker.getLatLng();    
    var zoom = gmap.getZoom();    
    
    $("input[name='" + gmap.map.elements_name + "[latitude]']").val(coords.lat());
    $("input[name='" + gmap.map.elements_name + "[longitude]']").val(coords.lng());   
    $("input[name='" + gmap.map.elements_name + "[zoom]']").val(zoom);
  } else {
    $("input[name='" + gmap.map.elements_name + "[latitude]']").val('');
    $("input[name='" + gmap.map.elements_name + "[longitude]']").val('');   
    $("input[name='" + gmap.map.elements_name + "[zoom]']").val('');
  }  
}

function sgHideInputs(elements_name) {
  $("input[name^='" + elements_name + "']").parent().hide();  
}

// accepts GMaps object or html dom id of an element
function sgAddMarker(map_id, coords) {  
  if (typeof map_id === "string") {   
    var gmap = window.simplest_gmaps[map_id];
  } else {
    gmap = map_id;
  } 
  
  
  coords = coords || gmap.getCenter();
  
  // we add updater handlers if we are in admin mode.
  // and we just render marker if we are in client side mode
  if (gmap.map.input_mode) {
    var marker = new GMarker(coords, {draggable:true});
    GEvent.addListener(marker, "dragend", function() {
      sgUpdateInputs(gmap);
    });
    
    GEvent.addListener(marker, "dragstart", function() {
      marker.closeInfoWindow();
    });
    
    GEvent.addListener(marker, "click", function() {
      marker.openInfoWindowHtml(Drupal.t('You can drag marker.<br /> You can also <a href="#" onclick="!code">delete it</a>', {'!code': "sgDeleteMarker('" + gmap.map.map_id + "'); return false;"}));
    });
    
    gmap.addOverlay(marker);    
    marker.openInfoWindowHtml(Drupal.t('You can drag marker.<br /> You can also <a href="#" onclick="!code">delete it</a>', {'!code': "sgDeleteMarker('" + gmap.map.map_id + "'); return false;"}));
  } else {
    var marker = new GMarker(coords);
    gmap.addOverlay(marker); 
  }
  
  gmap.map.marker = marker;  
  sgUpdateInputs(gmap);
  sgUpdateControls(gmap);
  
}

function sgDeleteMarker(map_id) {   
  
  if (typeof map_id === "string") {   
    var gmap = window.simplest_gmaps[map_id];
  } else {
    gmap = map_id;
  } 
  
  gmap.removeOverlay(gmap.map.marker);
  gmap.map.marker = null;
  sgUpdateInputs(gmap);
  sgUpdateControls(gmap);
}

function sgUpdateControls(gmap) { 
  if (!gmap.map.input_mode) return;
   
  if (!gmap.map.marker) {
    $('#' + gmap.map.map_id + '_controls').html('<div class="marker-add"><a href="#" onclick="sgAddMarker(\'' + gmap.map.map_id + '\');return false;">' + Drupal.t( 'Add marker here!' ) + '</a></div>');
  } else {
    $('#' + gmap.map.map_id + '_controls').html('<div class="marker-remove"><a href="#" onclick="sgDeleteMarker(\'' + gmap.map.map_id + '\');return false;">' + Drupal.t( 'Delete marker!' ) + '</a></div>');
  }
}

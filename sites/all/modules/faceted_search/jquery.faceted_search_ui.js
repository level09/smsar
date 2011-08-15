// $Id: jquery.faceted_search_ui.js,v 1.8 2008/01/17 20:50:16 davidlesieur Exp $

/**
 * Provides tooltips with subcategories when hovering categories, through AJAX.
 */ 
jQuery.facetedSearchUI = {
  anchor : null, // The hovered element.
  url : null, // The tooltip's url.
  id : null, // Unique id for the hovered category (used as cache key).
  cache : {}, // Cached HTML chunks.

  activate : function() {
    if (jQuery.facetedSearchUI.anchor != null || !this.href) {
      return; // Already active, or invalid anchor.
    }

    // Extract the facet key and id from the class of the category's parent.
    var facetKey = null;
    var facetId = null;
    jQuery(this).parents('.faceted-search-facet').each(function() {
      var matches = jQuery(this).attr('class').match(/faceted-search-facet--([^ ]+)--([^ ]+)/);
      if (matches.length > 0) {
        facetKey = matches[1];
        facetId = matches[2];
      }
    });
    if (!facetKey || !facetId) {
      return; // Could not find facet information.
    }

    // Derive the tooltip's url from the category's url, which also contains the search text.
    jQuery.facetedSearchUI.url = this.href.replace(/\/results\//, '/categories/' + facetKey + ':' + facetId + '/-/');
    if (jQuery.facetedSearchUI.url == this.href) {
      return; // Could not create the tooltip's url.
    }
    
    // Save the hovered element.
    jQuery.facetedSearchUI.anchor = this;
    jQuery.facetedSearchUI.id = null;

    // Wait a little bit before requesting the tooltip.
    window.setTimeout(function() { jQuery.facetedSearchUI.delayedActivate(jQuery.facetedSearchUI.url); }, 500);
  },
  
  delayedActivate : function(url) {
    // If tooltip has not been deactivated yet.
    if (jQuery.facetedSearchUI.url != null && jQuery.facetedSearchUI.url == url) {
      // Prepare the tooptip.
      var anchorPosition = jQuery(jQuery.facetedSearchUI.anchor).offset();
      var anchorWidth = jQuery(jQuery.facetedSearchUI.anchor).outerWidth();
      var windowWidth = jQuery(window).width();
      var tooltip = jQuery('#faceted-search-tooltip');
      var tooltipWidth = tooltip.outerWidth();
      tooltip.css('top', anchorPosition.top + 'px');
      if (anchorPosition.left + anchorWidth + tooltipWidth + 10 > windowWidth) {
        tooltip.css('left', (anchorPosition.left - tooltipWidth - 10) + 'px');
      }
      else {
        tooltip.css('left', (anchorPosition.left + anchorWidth + 10) + 'px');
      }

      // Show the tooltip.
      jQuery.facetedSearchUI.id = jQuery.facetedSearchUI.makeId(jQuery.facetedSearchUI.url);
      if (jQuery.facetedSearchUI.id in jQuery.facetedSearchUI.cache) { // Show from the cache.
        jQuery.facetedSearchUI.show(jQuery.facetedSearchUI.cache[jQuery.facetedSearchUI.id]);
        //jQuery(jQuery.facetedSearchUI.anchor).css('background', 'cyan'); // Debugging helper
      }
      else { // Was not in the cache, load it from the server.
        jQuery.get(jQuery.facetedSearchUI.url, null, jQuery.facetedSearchUI.load);
        //jQuery(jQuery.facetedSearchUI.anchor).css('background', 'yellow'); // Debugging helper
      }
    }
  },
  
  deactivate : function() {
    if (jQuery.facetedSearchUI.anchor != null) {
      jQuery.facetedSearchUI.anchor = null;
      jQuery.facetedSearchUI.id = null;
      jQuery.facetedSearchUI.url = null;
      jQuery('#faceted-search-tooltip').hide().empty();
    }
  },

  load : function(data) {
    data = Drupal.parseJson(data);
    if (data.id) {
      // Cache the received HTML chunk and show it.
      jQuery.facetedSearchUI.cache[data.id] = data.content;
      if (data.id == jQuery.facetedSearchUI.id) {
        jQuery.facetedSearchUI.show(data.content);
      }
    }
  },
  
  show : function(chunk) {
    if (chunk.length > 0) {
      // Received content is relevant to the currently hovered category, show it.
      jQuery('#faceted-search-tooltip').empty().append(chunk).show();
    }
  },

  makeId : function(url) {
    // Extract the search text to use as id.
    return decodeURIComponent(url.substr(url.lastIndexOf('/') + 1)).replace(/\+/g, ' ');
    // TODO: The above does not work properly when '/' is used in the search text.
  }
};

if (Drupal.jsEnabled) {
  jQuery(function() {
    // Insert the tooltip block.
    jQuery('body').append('<div id="faceted-search-tooltip"></div>');
    // Bind hover behavior on category links.
    jQuery('.faceted-search-category a').hover(jQuery.facetedSearchUI.activate, jQuery.facetedSearchUI.deactivate);
    // Bind click behavior to force closing the tooptip, if ever needed.
    jQuery(this).click(jQuery.facetedSearchUI.deactivate);
  });
}


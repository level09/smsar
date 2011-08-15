<?php
// $ID: $
/**
 * Helper functions for imagecache_textactions
 * 
 * Static rendertext and dynamic caption functions
 * 
 * This is a rewrite of textactions to consolidate various text effects into a
 * new SVG-like syntax. Not compatible with earlier efforts. HOPEFULLY
 * compatible with broader imageapi efforts and signwriter, menuwriter usage
 * etc.
 * Quite simply, CSS is not good enough a syntax to do half the effects we want.
 * That's why we are doing images, right?
 * SVG however does have enough bells to make this worthwhile.
 * 
 * Contains a stub for imageapi functions : imageapi_image_create_text that may
 * be ported over to there if that makes sense.
 */

require_once(dirname(__FILE__) . '/textactions.inc');

/**
 * Place text on top of the current canvas
 *
 * Implementation of imagecache_hook_form()
 *
 * @param $action array of settings for this action
 * @return a form definition
 */
function textactions_rendertext_form($action) {

  $defaults = array(
    'xpos' => 'left+10',
    'ypos' => 'bottom-10',
    'textstyle' => array(
      'fontfile' => drupal_get_path('module', 'imageapi_text') . '/fonts/liberation-fonts-1.04/LiberationSans-Regular.ttf',
      'style' => "font-size:12px;\nfill:#333333;\ntop:10px;\nright:10px;",
    ),
    'text' => 'Hello World!',
    'evaluate_text' => FALSE,
  );
  // Our 'textstyle' parameter is a nested array - reflecting the wiget fieldset structure
  // only because imagecache sets the form
  // #tree to true, and unsetting it doesn't work.
  
  $action = array_merge($defaults, (array)$action);
  $form = array(
    'textstyle' => imageapi_text_style_widget($action['textstyle']),
    
    'text' => array(
      '#type' => 'textarea',
      '#rows' => 7,
      '#title' => t('Text'),
      '#default_value' => $action['text'],
      '#description' => t('The text string. If you are using a WYSIWYG editor, you should disable it for this field!'),
      '#wysiwyg' => FALSE,
    ),
    'evaluate_text' => array(
      '#type' => 'checkbox',
      '#title' => t('Evaluate text as PHP code'),
      '#default_value' => $action['evaluate_text'],
      '#description' => t('If selected, the text will be treated as PHP code.
      <p>
        Enter PHP code that will <b>return</b> your dynamic text. Do not use %php tags. 
        <br />EG <code>return format_date(time());</code>  
        <br /><code>return $file_data->description ? $file_data->description : $node->title;</code>
      </p>
      <p>Note that executing incorrect PHP-code can break your Drupal site.</p>
      <p>
        <b>If it\'s an image.module image</b> then a <b>$node</b> object with its values 
        <em>may</em> be available.
        <br/><code>return $node->title;</code>
        <br/><code>return format_date($node->created);</code>
      </p>

      <p>
        If it\'s an image that has been attached to a node using <b>CCK-filefield-imagefield</b>
        (or just filefield)
        then as well as the parent <b>$node</b> object,
        a <b>$file_data</b> object that may contain a file description from that file field.
        <br/><code>return $file_data->description;</code>
        <small>So far that seems to be the only available \'data\' provided by filefield, 
        but you can investigate the node structure using devel.module or print_r() 
        to see what else this array actually contains</small>.
      </p>
      <p>
        If it\'s a file that\'s just been <b>attached using upload.module</b>,
        a <b>$file_data</b> object may also have a description.
        <br/><code>return $file_data->description;</code>
      </p>
      <p>
        If the image path is detected as belonging to more than one node, just the
        data for the first one found is returned. 
      </p>

      <p>
        An "<b>$image</b>" object is also available, but that usually contains only technical data, including 
        <br/><code>return $image->source;</code>
        <br/><code>return basename($image->source);</code>
        <br/><code>return $image->info["filesize"];</code>
      </p>
      ', 
      array('%php' => '<?php ?>'))
    ),
  );
  if (! user_access('administer site configuration')) {
    $form['evaluate_text']['#disabled'] = TRUE;
    $form['text']['#disabled'] = $action['evaluate_text']; // lock this if an admin has enabled evaluation.
    $form['evaluate_text']['#description'] = 'requires <b>administer site configuration</b> permissions.';
  }
  #$form['#tree'] = FALSE;
  #$form['textstyle']['#tree'] = FALSE;

  return $form;
}



/**
 * Implementation of theme_hook() for imagecache_ui.module
 */
function theme_textactions_rendertext($element) {
  $data = $element['#value'];
  $style_atts = imageapi_text_parse_style($data['textstyle']['style']);
  return "<em><strong>{$data['text']}</strong></em><br/>{$data['textstyle']['style']}" ;
}


/**
 * Place the source image on the current background
 *
 * Implementation of hook_image()
 *
 * @param $image
 * @param $action
 */
function textactions_rendertext_image(&$image, $action = array()) {

  if (!empty($action['evaluate_text'])) {
    $text = textactions_evaluate_text($image, $action);
  }
  else {
    $text = $action['text'];
  }

  $style = imageapi_text_parse_style($action['textstyle']['style']);
  $fontfile = $action['textstyle']['fontfile'];

  // Calculate position by first creating a temp image 
  // containing the text and accessing its dimensions

  // $textimage is a placeholder that will get the image created in it - to work like the rest of the imageapi functions.
  // We really need to force the toolkit to GD, but even then it can't be merged back into imagemagick
  $textimage = (object) array(
    'toolkit' => $image->toolkit,
  );
  
  if(! imageapi_image_create_text($textimage, $text, $fontfile, $style) ) {
    drupal_set_message(t('Failed to generate text image using %toolkit. Not overlaying text.', array('%toolkit' => $textimage->toolkit )), 'error');
    return FALSE;
  }

  // $textimage should now have its size info available.
  // Calc the position
  if (!empty($style['top'])) {
    $ypos = $style['top'];
  }
  if (!empty($style['bottom'])) {
    $ypos = $image->info['height'] - ( $textimage->info['height'] + $style['bottom']);
  }
  if (! isset($ypos)) {
    // assume center
    $ypos = ($image->info['height']/2) - ($textimage->info['height']/2);
  }

  if (!empty($style['left'])) {
    $xpos = $style['left'];
  }
  if (!empty($style['right'])) {
    $xpos = $image->info['width'] - ( $textimage->info['width'] + $style['right']);
  }
  if (! isset($xpos)) {
    // assume center
    $xpos = ($image->info['width']/2) - ($textimage->info['width']/2);
  }
  
  return imageapi_image_overlay($image, $textimage, $xpos, $ypos);
  
  ////////////////////////////////

}


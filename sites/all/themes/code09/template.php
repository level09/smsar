<?php

function c($var){
	
	drupal_set_message('<pre>' .print_r($var,1) .'</pre>');
}


function code09_preprocess_node(&$vars, $hook) {
  

$vars['node_bottom'] = theme('blocks', 'node_bottom');
  
    
}




//render taxonomy terms based on vocabid

function render_terms($node,$vocabid,$title=null,$divclass=null) {

$terms = taxonomy_node_get_terms_by_vocabulary($node, $vocabid );
if ($terms) {

$output = '<div class="field field-item'. ' '.$divclass .'"><div class="field-label-inline-first">' .$title .' </div> ' ;
$term_links = array();
foreach ($terms as $term) {
$term_links[] = l($term->name, taxonomy_term_path($term), array('attributes' => array('rel' => 'tag', 'title' => strip_tags($term->description)))) ;
$output .= implode(", ", $term_links);
}
}
$output .='</div>';
return ($output);
}

//render any field, (multi-values can have a limit, 0 for no limit)
function render_field($node,$name,$title=null,$divclass=null,$limit=0,$start=0) 
{


$field_name = 'field_'.$name;
$field = $node->{$field_name};

$limit = ($limit == 0) ? count($field): $limit;

for ($i=$start; $i < $limit; $i++) {
$output .= '<div class="field field-item'. ' '.$divclass .'"><div class="field-label-inline-first">' .$title .' </div> ' ;
$output .= $field[$i]['view'];

$output .='</div>';
}
return ($output);
}


function render_hierarchial_terms($node,$vocabid,$title=null,$divclass=null) {

$terms = taxonomy_node_get_terms_by_vocabulary($node, $vocabid );
if ($terms) {

$output = '<div class="field field-item'. ' '.$divclass .'"><div class="field-label-inline-first">' .$title .' </div> ' ;
$term_links = array();
foreach ($terms as $term) {
$term_links[] = l($term->name, taxonomy_term_path($term), array('attributes' => array('rel' => 'tag', 'title' => strip_tags($term->description)))) ;

}
}
$output  .= implode(', ',$term_links);	
$output .='</div>';

return ($output);
}
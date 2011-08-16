<?php 
// $Id: node.tpl.php,v 1.1 2008/10/01 03:26:19 jwolf Exp $ 
?>

<?php if($page !=0) { ?>


<!-- start node.tpl.php -->
<div id="node-<?php print $node->nid; ?>" class="node <?php print $node_classes; ?>">
  <?php print $picture ?>

  <?php if ($page == 0): ?>
  <h2 class="title"><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
  <?php endif; ?>

  <div class="meta">
    <?php if ($submitted): ?>
    <span class="submitted"><?php print $submitted ?></span>
    <?php endif; ?>
  </div>

  <div class="content">
    <?php print $content ?>
  </div>

  <?php if ($terms): ?>
  <div class="terms">
    <?php print $terms; ?>
  </div>
  <?php endif;?>
  
  <?php if ($links): ?>
  <div class="links">
    <?php print $links; ?>
  </div>
  <?php endif; ?>

  <?php if ($node_bottom && !$teaser): ?>
  <div id="node-bottom">
    <?php print $node_bottom; ?>
  </div>
  <?php endif; ?>
</div>
<!-- /#node-<?php print $node->nid; ?> -->
<?php } else {  ?>
<div class="property-block">
<div class="node-top">
<div style="float:left">
<?php
$terms = taxonomy_node_get_terms_by_vocabulary($node,6);
foreach($terms as $term)
{
echo l($term->name,'taxonomy/term/' .$term->tid);
	
}
echo ' in ';

$terms = taxonomy_node_get_terms_by_vocabulary($node,3);
foreach($terms as $term)
{
echo l($term->name,'taxonomy/term/' .$term->tid);
echo ' ';
}
?>

</div>
<div style="float:right;text-align:right;font-weight:bold;color:#fff;">
<?php echo $node->field_price[0]['view']; ?>
</div>
</div>
<div style="float:left;display:block;padding-top:10px;padding-left:10px">
<?php echo $node->field_gallery[0]['view']; ?>
</div>
<div style="float:right;width:340px;padding:15px;">
<h2 class="title"><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<div> <?php echo node_teaser($node->content['body']['#value'],null, 100); ?>  </div>
<ul class="teaser-links">
<li><?php $terms = taxonomy_node_get_terms_by_vocabulary($node,7);
foreach($terms as $term)
{
echo l("Rooms: $term->name",'taxonomy/term/' .$term->tid);	
}
?>
</li>
<li><?php echo $node->field_area[0]['view']; ?></li>

<li><?php    echo l('Contact Agent','node/' .$node->nid  ,array('fragment'=>'block-authorcontact-0')) ;?></li>
<li><?php echo l('Full Details','node/' .$node->nid) ;?></li>

</ul>

</div>


</div>


 <?php }?>
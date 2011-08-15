<?php 
// $Id: node.tpl.php,v 1.1 2008/10/01 03:26:19 jwolf Exp $ 
?>

<?php if($page !=0) { ?>
	  <h2 class="title"><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>


	<div id="general">
	<div class="span-4 column">
	<?php echo render_field($node,'gallery','','pimage',1) ?>
	</div>
	<div class="span-8 last">
	<?php echo render_field($node,'price','','price') ?>
	
	
	<?php echo render_terms($node,7,'Rooms: ',null) ?>
	<?php echo render_hierarchial_terms($node,3,'Location: ','') ?>
	<?php echo render_terms($node,2,'Status : ','status') ?>
	<?php echo render_terms($node,6,'Type : ','type') ?>
	<?php echo render_field($node,'area','Approx. size: ','area') ?>
	<?php echo render_field($node,'reference','Broker Reference','reference') ?>
	</div>
	</div>
	




<div class="piece">
<fieldset>
<legend>Image Gallery</legend>
<div class="gallery">
<?php echo render_field($node,'gallery',null,'pimage',0,1) ?>
</div>
</fieldset>
</div>


<div class="piece">
<fieldset>
<legend>Property Overview</legend>
<div class="gallery">
<?php echo $node->content['body']['#value'];?>
</div>
</fieldset>
</div>

<div>
<fieldset class="piece">
<legend>More Options</legend>
<div class="more">
<?php echo $links;?>
</div>
</fieldset>
</div>
  <?php if ($node_bottom ): ?>
  <div id="node-bottom" class="region region-node-bottom"> <?php print $node_bottom; ?> </div>
  <!-- /#node-advert-top -->
  <?php endif; ?>




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
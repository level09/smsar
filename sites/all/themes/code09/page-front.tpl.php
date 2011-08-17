<?php include_once('head.php'); ?>
<body>

	<a href="http://github.com/level09/smsar"><img style="position: absolute; top: 0; left: 0; border: 0;" src="https://a248.e.akamai.net/assets.github.com/img/6429057dfef9e98189338d22e7f6646c6694f032/687474703a2f2f73332e616d617a6f6e6177732e636f6d2f6769746875622f726962626f6e732f666f726b6d655f6c6566745f6f72616e67655f6666373630302e706e67" alt="Fork me on GitHub"></a>



<div id="topstrip"></div>
<div id="headerwrap">
  <div id="header" class="container">
    <div id="logo">
    <?php
     $img =   theme_image(path_to_theme() .'/images/logo.png');
	print   l($img,'',array('html' => true));

	   ?>
    </div>
    <div id="nav">
      <?php echo menu_tree($menu_name='primary-links'); ?>
    </div>
    <!-- Nav //-->
  </div>
  <!-- header //-->
</div>
<!-- header wrap//-->
<?php include_once('banner.php'); ?>
<div id="page" class="container">
  <div id="main" class="span-15 column">
    <div class="main-pad"> 
    	
    	<?php echo $content_top; ?>
    	
    	<?php  if($breadcrumb)  print $breadcrumb; ?>
           <?php if ($tabs): print '<div id="tabs-wrapper" class="clear-block">'; endif; ?>
        <?php if ($title): print '<h2'. ($tabs ? ' class="with-tabs"' : '') .'>'. $title .'</h2>'; endif; ?>
        <?php if ($tabs): print '<ul class="tabs primary">'. $tabs .'</ul></div>'; endif; ?>
        <?php if ($tabs2): print '<ul class="tabs secondary">'. $tabs2 .'</ul>'; endif; ?>
        <?php if ($show_messages && $messages): ?> <div><?php print $messages;?></div> <?php endif; ?>
        <?php print $help;  ?>
                  <?php

		$view = views_get_view('Featured_Property');
		
		

		$view->render();
		
		if(count($view->result) >0)
		{
			echo '<h3> Featured Properties </h3>';
		$i = 1;
		$cols = 2;  
		echo '<table id="featured"  border="0" cellpadding="2" cellspacing="3" style="padding: 9px;" ><tr>';
		
	
		foreach($view->result as $result)
		
		{	
		
			echo '<td>';
                
			$node =  node_load($result->nid);


			echo '<div class="featured-node">';
			echo  l($node->title,'node/' .$node->nid);
			echo '<div class="featured-image">';
			echo theme_imagecache('medium',$node->field_gallery[0]['filepath'],'','');
			echo '</div>';
			echo '<div class="featured-body"';
			echo '<div style="margin-bottom:7 px"><b>';
			
			echo  '<div style="color:#666;font-size:14px;	text-decoration: line-through"> Market Price: ' .$node->field_oprice[0]['value'] .' AED </div>';
			echo  '<div style="color:#446600;font-size:14px;">Our Price: ' .$node->field_price[0]['value'] .' AED </div>';
			echo '</div></b>';
			
						
			
		
			
			
			
			echo '</div>';
		
			echo '</div>';
			echo '</td>';
if($i%$cols == 0) echo '</tr><tr>';
$i++;

		}
		
		echo '</table>';
		}
		
		
   
      ?>
      
      <?php 
	  $view = views_get_view('Random_Property');
	  
	  $view_args = array();
	  $display_id = 'default';
	  echo '<h3 style="margin-top:20px"> Selected Properties </h3>';
	  echo $view->execute_display($display_id , $view_args);
	  
	  
	  ?>
        
        
    
    </div>
  </div>
  <div class="span-9 last" id="sidebar">
  
  
 	 <div class="sidebar-pad">
  <div class="inner-pad">
    <?php print $right; ?>
    </div>
  	</div>
  </div>
</div>
<div id="footerwrap">
  <div id="footer" class="container">
    <p>copyright (c) 2009 - Developed by level09 studios</p>
  </div>
</div>
<?php print $closure; ?>
</body>
</html>

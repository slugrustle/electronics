// This file is meant for use with OpenSCAD, a
// constructive solid geometry 3D CAD program.
// See https://www.openscad.org/
// The height dimensions have been rounded to best
// support a 0.14mm layer height.
$fa = 3;
$fs = 0.05;

$wallthick = 3;
$bottomthick = 4.06;
$screwblind = 1.68;
$standoffheight = 2.94;
$standoff_inner_radius = 1.0;
$standoff_outer_radius = 2.9;
$boardwidth = 110;
$boarddepth = 61;
$boardoffset = 1;
$boardthick = 1.6;
$insertholedia = 3.6;
$insertholedepth = 5.6;
$cornerdia = 2*$wallthick+$insertholedia;
// Extra $wallthick in $depth is because connectors hang one
// $wallthick plus one $boardoffset off front edge of board
$depth = $wallthick + 2*$boardoffset + $boarddepth + $cornerdia;
$width = 2*$boardoffset + $boardwidth + $cornerdia;
$height = $bottomthick + $standoffheight + $boardthick + 20.02;
$standoff1_x = 0.5*$cornerdia + $boardoffset + 11;
$standoff1_y = 0.5*$cornerdia + $boardoffset + 6;
$standoff2_x = 0.5*$cornerdia + $boardoffset + 31;
$standoff2_y = 0.5*$cornerdia + $boardoffset + 6;
$standoff3_x = 0.5*$cornerdia + $boardoffset + 52.3;
$standoff3_y = 0.5*$cornerdia + $boardoffset + 10;
$standoff4_x = 0.5*$cornerdia + $boardoffset + 97.7;
$standoff4_y = 0.5*$cornerdia + $boardoffset + 10;
$standoff5_x = 0.5*$cornerdia + $boardoffset + 14;
$standoff5_y = 0.5*$cornerdia + $boardoffset + 56.2;
$standoff6_x = 0.5*$cornerdia + $boardoffset + 96;
$standoff6_y = 0.5*$cornerdia + $boardoffset + 56.2;
$dcjack_x = 0.5*$cornerdia + $boardoffset + 15;
$dcjack_z = $bottomthick + $standoffheight + $boardthick - 0.62;
$dcjackwidth = 10;
$dcjackheight = 12.18;
$termsocket_x = 0.5*$cornerdia + $boardoffset + 54.18;
$termsocket_z = $bottomthick + $standoffheight + $boardthick - 0.62;
$termsocketwidth = 41.64;
$termsocketheight = 9.66;

difference()
{
  union()
  {
    // Corner pillars
    translate([0.5*$cornerdia, 0.5*$cornerdia, 0])
    {
      cylinder(d=$cornerdia, h=$height);
    }

    translate([$width-0.5*$cornerdia, 0.5*$cornerdia, 0])
    {
      cylinder(d=$cornerdia, h=$height);
    }

    translate([0.5*$cornerdia, $depth-0.5*$cornerdia, 0])
    {
      cylinder(d=$cornerdia, h=$height);
    }

    translate([$width-0.5*$cornerdia, $depth-0.5*$cornerdia, 0])
    {
      cylinder(d=$cornerdia, h=$height);
    }
    
    // Walls
    translate([0.5*$cornerdia, 0, 0])
    {
      cube([$boardwidth+2*$boardoffset, $wallthick, $height]);
    }
    
    translate([0, 0.5*$cornerdia, 0])
    {
      cube([$wallthick, $boarddepth+2*$boardoffset+$wallthick, $height]);
    }
    
    translate([0.5*$cornerdia, $depth - $wallthick, 0])
    {
      cube([$boardwidth+2*$boardoffset, $wallthick, $height]);
    }
    
    translate([$width - $wallthick, 0.5*$cornerdia, 0])
    {
      cube([$wallthick, $boarddepth+2*$boardoffset+$wallthick, $height]);
    }
    
    // Bottom plate
    translate([0.5*$wallthick, 0.5*$wallthick, 0])
    {
      cube([$width - $wallthick, $depth - $wallthick, $bottomthick]);
    }
    
    // Board standoffs
    translate([$standoff1_x, $standoff1_y, $bottomthick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$standoff2_x, $standoff2_y, $bottomthick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$standoff3_x, $standoff3_y, $bottomthick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$standoff4_x, $standoff4_y, $bottomthick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$standoff5_x, $standoff5_y, $bottomthick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$standoff6_x, $standoff6_y, $bottomthick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
  }
  
  // Standoff screw holes
  translate([$standoff1_x, $standoff1_y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$bottomthick+$standoffheight-$screwblind);
  }
  
  translate([$standoff2_x, $standoff2_y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$bottomthick+$standoffheight-$screwblind);
  }
  
  translate([$standoff3_x, $standoff3_y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$bottomthick+$standoffheight-$screwblind);
  }
  
  translate([$standoff4_x, $standoff4_y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$bottomthick+$standoffheight-$screwblind);
  }
  
  translate([$standoff5_x, $standoff5_y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$bottomthick+$standoffheight-$screwblind);
  }
  
  translate([$standoff6_x, $standoff6_y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$bottomthick+$standoffheight-$screwblind);
  }
  
  // Holes for lid screw threaded inserts
  translate([0.5*$cornerdia, 0.5*$cornerdia, $height - $insertholedepth])
  {
    cylinder(d=$insertholedia, h=$insertholedepth);
  }
  
  translate([$width-0.5*$cornerdia, 0.5*$cornerdia, $height - $insertholedepth])
  {
    cylinder(d=$insertholedia, h=$insertholedepth);
  }

  translate([0.5*$cornerdia, $depth-0.5*$cornerdia, $height - $insertholedepth])
  {
    cylinder(d=$insertholedia, h=$insertholedepth);
  }

  translate([$width-0.5*$cornerdia, $depth-0.5*$cornerdia, $height - $insertholedepth])
  {
    cylinder(d=$insertholedia, h=$insertholedepth);
  }
  
  // Front wall opening for DC jack
  translate([$dcjack_x, 0, $dcjack_z])
  {
    cube([$dcjackwidth, $wallthick, $dcjackheight]);
  }
  
  // Front wall opening for terminal block socket
  translate([$termsocket_x, 0, $termsocket_z])
  {
    cube([$termsocketwidth, $wallthick, $termsocketheight]);
  }
}
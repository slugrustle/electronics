// This file is meant for use with OpenSCAD, a
// constructive solid geometry 3D CAD program.
// See https://www.openscad.org/
// The height dimensions have been rounded to best
// support a 0.14mm layer height.
$fa = 3;
$fs = 0.05;

$wallthick = 3;
$bottomthick = 4.34;
$insertholedia = 3.6;
$insertholedepth = 5.6;
$cornerdia = 2*$wallthick+$insertholedia;
$boardcornerrad = 6;
$screwblind = 0.7;
$standoffheight = 2.94;
$standoff_inner_radius = 0.8;
$standoff_outer_radius = 2.9;
$boardwidth = 110;
$boarddepth = 61.8;
$boardoffsetfront = 0.5*$cornerdia - 0.8;
// $boardoffsetback has extra margin because the connectors
// hang off the front edge of the board by 4mm
$boardoffsetback = 0.5*$cornerdia + 2.8;
$boardoffsetside = 0.5*$cornerdia;
$boardthick = 1.6;
$depth = $boardoffsetfront + $boardoffsetback + $boarddepth;
$width = 2*$boardoffsetside + $boardwidth;
$height = $bottomthick + $standoffheight + $boardthick + 20.02;
$standoff1_boardx = 11;
$standoff1_boardy = 6.8;
$standoff2_boardx = 31;
$standoff2_boardy = 6.8;
$standoff3_boardx = 52.3;
$standoff3_boardy = 10;
$standoff4_boardx = 97.7;
$standoff4_boardy = 10;
$standoff5_boardx = 14;
$standoff5_boardy = 57;
$standoff6_boardx = 96;
$standoff6_boardy = 57;
$standoff1_x = $boardoffsetside + $standoff1_boardx;
$standoff1_y = $boardoffsetfront + $standoff1_boardy;
$standoff2_x = $boardoffsetside + $standoff2_boardx;
$standoff2_y = $boardoffsetfront + $standoff2_boardy;
$standoff3_x = $boardoffsetside + $standoff3_boardx;
$standoff3_y = $boardoffsetfront + $standoff3_boardy;
$standoff4_x = $boardoffsetside + $standoff4_boardx;
$standoff4_y = $boardoffsetfront + $standoff4_boardy;
$standoff5_x = $boardoffsetside + $standoff5_boardx;
$standoff5_y = $boardoffsetfront + $standoff5_boardy;
$standoff6_x = $boardoffsetside + $standoff6_boardx;
$standoff6_y = $boardoffsetfront + $standoff6_boardy;
$dcjack_x = $boardoffsetside + 15;
$dcjack_z = $bottomthick + $standoffheight + $boardthick - 0.62;
$dcjackwidth = 10;
$dcjackheight = 12.18;
$termsocket_x = $boardoffsetside + 54.18;
$termsocket_z = $bottomthick + $standoffheight + $boardthick - 0.62;
$termsocketwidth = 41.64;
$termsocketheight = 9.66;

// circuit board, for debugging. prepended with *
// so it doesn't show up during STL export.
*translate([$boardoffsetside, $boardoffsetfront, $bottomthick+$standoffheight])
{
  difference()
  {
    cube([$boardwidth, $boarddepth, $boardthick]);
    cube([$boardcornerrad, 0.8, $boardthick]);
    translate([$boardwidth-$boardcornerrad, 0, 0])
    {
      cube([$boardcornerrad, 0.8, $boardthick]);
    }
    translate([0, 0.8, 0])
    {
      cylinder(r=$boardcornerrad, h=$boardthick);
    }
    translate([$boardwidth, 0.8, 0])
    {
      cylinder(r=$boardcornerrad, h=$boardthick);
    }
    translate([0, $boarddepth, 0])
    {
      cylinder(r=$boardcornerrad, h=$boardthick);
    }
    translate([$boardwidth, $boarddepth, 0])
    {
      cylinder(r=$boardcornerrad, h=$boardthick);
    }
    translate([$standoff1_boardx, $standoff1_boardy, 0])
    {
      cylinder(r=$standoff_inner_radius, h=$boardthick);
    }
    translate([$standoff2_boardx, $standoff2_boardy, 0])
    {
      cylinder(r=$standoff_inner_radius, h=$boardthick);
    }
    translate([$standoff3_boardx, $standoff3_boardy, 0])
    {
      cylinder(r=$standoff_inner_radius, h=$boardthick);
    }
    translate([$standoff4_boardx, $standoff4_boardy, 0])
    {
      cylinder(r=$standoff_inner_radius, h=$boardthick);
    }
    translate([$standoff5_boardx, $standoff5_boardy, 0])
    {
      cylinder(r=$standoff_inner_radius, h=$boardthick);
    }
    translate([$standoff6_boardx, $standoff6_boardy, 0])
    {
      cylinder(r=$standoff_inner_radius, h=$boardthick);
    }
  }
}

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
      cube([$width-$cornerdia, $wallthick, $height]);
    }
    
    translate([0, 0.5*$cornerdia, 0])
    {
      cube([$wallthick, $depth-$cornerdia, $height]);
    }
    
    translate([0.5*$cornerdia, $depth - $wallthick, 0])
    {
      cube([$width-$cornerdia, $wallthick, $height]);
    }
    
    translate([$width - $wallthick, 0.5*$cornerdia, 0])
    {
      cube([$wallthick, $depth-$cornerdia, $height]);
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
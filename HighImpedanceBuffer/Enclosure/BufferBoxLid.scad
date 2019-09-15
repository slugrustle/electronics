// This file is meant for use with OpenSCAD, a
// constructive solid geometry 3D CAD program.
// See https://www.openscad.org/
// The height dimensions have been rounded to best
// support a 0.14mm layer height.
$fa = 3;
$fs = 0.05;

$lipthick = 2.1;
$lipinset = 0.5;
$platethick = 4.06;
$lidholedia = 3.2;
$labelthick = 0.42;
$labelwidth = 81.28;
$labeldepth = 30.48;

$wallthick = 3;
$insertholedia = 3.6;
$cornerdia = 2*$wallthick+$insertholedia;
$boardwidth = 110;
$boarddepth = 61.8;
$boardoffsetfront = 0.5*$cornerdia - 0.8;
// $boardoffsetback has extra margin because the connectors
// hang off the front edge of the board by 4mm
$boardoffsetback = 0.5*$cornerdia + 2.8;
$boardoffsetside = 0.5*$cornerdia;
$depth = $boardoffsetfront + $boardoffsetback + $boarddepth;
$width = 2*$boardoffsetside + $boardwidth;

union()
{
  // Top plate
  translate([0,0,$lipthick])
  {
    difference()
    {
      union()
      {
        // Corner pillars
        translate([0.5*$cornerdia, 0.5*$cornerdia, 0])
        {
          cylinder(d=$cornerdia, h=$platethick);
        }

        translate([$width-0.5*$cornerdia, 0.5*$cornerdia, 0])
        {
          cylinder(d=$cornerdia, h=$platethick);
        }

        translate([0.5*$cornerdia, $depth-0.5*$cornerdia, 0])
        {
          cylinder(d=$cornerdia, h=$platethick);
        }

        translate([$width-0.5*$cornerdia, $depth-0.5*$cornerdia, 0])
        {
          cylinder(d=$cornerdia, h=$platethick);
        }
          
        // Walls
        translate([0.5*$cornerdia, 0, 0])
        {
          cube([$width-$cornerdia, $wallthick, $platethick]);
        }
        
        translate([0, 0.5*$cornerdia, 0])
        {
          cube([$wallthick, $depth-$cornerdia, $platethick]);
        }
        
        translate([0.5*$cornerdia, $depth - $wallthick, 0])
        {
          cube([$width-$cornerdia, $wallthick, $platethick]);
        }
        
        translate([$width - $wallthick, 0.5*$cornerdia, 0])
        {
          cube([$wallthick, $depth-$cornerdia, $platethick]);
        }
          
        // Most of plate
        translate([0.5*$wallthick, 0.5*$wallthick, 0])
        {
          cube([$width - $wallthick, $depth - $wallthick, $platethick]);
        }
      }
      
      // Holes for lid screws
      translate([0.5*$cornerdia,0.5*$cornerdia,0])
      {
        cylinder(d=$lidholedia,h=$platethick);
      }
      translate([$width-0.5*$cornerdia,0.5*$cornerdia,0])
      {
        cylinder(d=$lidholedia,h=$platethick);
      }
      translate([0.5*$cornerdia,$depth-0.5*$cornerdia,0])
      {
        cylinder(d=$lidholedia,h=$platethick);
      }
      translate([$width-0.5*$cornerdia,$depth-0.5*$cornerdia,0])
      {
        cylinder(d=$lidholedia,h=$platethick);
      }
      
      // Inset for label
      translate([0.5*($width-$labelwidth),0.5*($depth-$labeldepth),$platethick-$labelthick])
      {
        cube([$labelwidth,$labeldepth,$labelthick]);
      }
    }
  }
  
  // Lid lip
  difference()
  {
    union()
    {
      // Straight parts
      translate([$cornerdia+$lipinset,$wallthick+$lipinset,0])
      {
        cube([$width-2*$cornerdia-2*$lipinset,$lipthick,$lipthick]);
      }
      
      translate([$cornerdia+$lipinset,$depth-$wallthick-$lipinset-$lipthick,0])
      {
        cube([$width-2*$cornerdia-2*$lipinset,$lipthick,$lipthick]);
      }
      
      translate([$wallthick+$lipinset,$cornerdia+$lipinset,0])
      {
        cube([$lipthick,$depth-2*$cornerdia-2*$lipinset,$lipthick]); 
      }
      
      translate([$width-$wallthick-$lipinset-$lipthick,$cornerdia+$lipinset,0])
      {
        cube([$lipthick,$depth-2*$cornerdia-2*$lipinset,$lipthick]); 
      }
      
      // Massively overfilled corner rounds
      translate([0.5*$cornerdia, 0.5*$cornerdia, 0])
      {
        cylinder(r=0.5*$cornerdia+$lipinset+$lipthick,h=$lipthick);
      }
      
      translate([$width-0.5*$cornerdia, 0.5*$cornerdia, 0])
      {
        cylinder(r=0.5*$cornerdia+$lipinset+$lipthick,h=$lipthick);
      }
      
      translate([0.5*$cornerdia, $depth-0.5*$cornerdia, 0])
      {
        cylinder(r=0.5*$cornerdia+$lipinset+$lipthick,h=$lipthick);
      }
      
      translate([$width-0.5*$cornerdia, $depth-0.5*$cornerdia, 0])
      {
        cylinder(r=0.5*$cornerdia+$lipinset+$lipthick,h=$lipthick);
      }
    }
    // Cut out corners
    translate([0.5*$cornerdia, 0.5*$cornerdia, 0])
    {
      cylinder(r=0.5*$cornerdia+$lipinset,h=$lipthick);
    }
    translate([-$lipinset-$lipthick,-$lipinset-$lipthick,0])
    {
      cube([2*(0.5*$cornerdia+$lipinset+$lipthick),$wallthick+2*$lipinset+$lipthick,$lipthick]);
    }
    translate([-$lipinset-$lipthick,-$lipinset-$lipthick,0])
    {
      cube([$wallthick+2*$lipinset+$lipthick,2*(0.5*$cornerdia+$lipinset+$lipthick),$lipthick]);
    }
    
    translate([$width-0.5*$cornerdia, 0.5*$cornerdia, 0])
    {
      cylinder(r=0.5*$cornerdia+$lipinset,h=$lipthick);
    }
    translate([$width-$cornerdia-$lipinset-$lipthick,-$lipinset-$lipthick,0])
    {
      cube([2*(0.5*$cornerdia+$lipinset+$lipthick),$wallthick+2*$lipinset+$lipthick,$lipthick]);
    }
    translate([$width-$wallthick-$lipinset,-$lipinset-$lipthick,0])
    {
      cube([$wallthick+2*$lipinset+$lipthick,2*(0.5*$cornerdia+$lipinset+$lipthick),$lipthick]);
    }
    
    translate([0.5*$cornerdia, $depth-0.5*$cornerdia, 0])
    {
      cylinder(r=0.5*$cornerdia+$lipinset,h=$lipthick);
    }
    translate([-$lipinset-$lipthick,$depth-$wallthick-$lipinset,0])
    {
      cube([2*(0.5*$cornerdia+$lipinset+$lipthick),$wallthick+2*$lipinset+$lipthick,$lipthick]);
    }
    translate([-$lipinset-$lipthick,$depth-$cornerdia-$lipinset-$lipthick,0])
    {
      cube([$wallthick+2*$lipinset+$lipthick,2*(0.5*$cornerdia+$lipinset+$lipthick),$lipthick]);
    }
    
    translate([$width-0.5*$cornerdia, $depth-0.5*$cornerdia, 0])
    {
      cylinder(r=0.5*$cornerdia+$lipinset,h=$lipthick);
    }
    translate([$width-$cornerdia-$lipinset-$lipthick,$depth-$wallthick-$lipinset,0])
    {
      cube([2*(0.5*$cornerdia+$lipinset+$lipthick),$wallthick+2*$lipinset+$lipthick,$lipthick]);
    }
    translate([$width-$wallthick-$lipinset,$depth-$cornerdia-$lipinset-$lipthick,0])
    {
      cube([$wallthick+2*$lipinset+$lipthick,2*(0.5*$cornerdia+$lipinset+$lipthick),$lipthick]);
    }
  }
}
// This file is meant for use with OpenSCAD, a
// constructive solid geometry 3D CAD program.
// See https://www.openscad.org/

// The height dimensions have been rounded to best
// support a 0.14mm layer height.

// Render accuracy settings
$fa = 3;
$fs = 0.05;

// These numbers define the object dimensions.
// Units: mm
$boardwidth = 178;
$boarddepth = 72;
$screw2holedia = 1.6;
$screw2holedepth = 5.25;
$screw2supportdia = 5.8;
$screw4holedia = 2.2;
$screw4holedepth = 5.25;
$screw4supportdia = 7.5;
$bosssupoortdia = 6;
$bosspostdia = 3;
$bosspostheight = 1.96;
$supportheight = 2.94;
$platethick = 2.94;
$borderwidth = 2.94;

$screw2locs = [[135.5, 5, 0], [5, 5, 0], [103, 5, 0], [37, 5, 0], [173.4, 5, 0], [70, 5, 0]];

$screw4locs = [[41, 19.5, 0], [132, 19.5, 0], [41, 65.5, 0], [132, 65.5, 0], [10, 62, 0], [168, 62, 0]];

$bosslocs = [[151.5, 11, 0], [86.5, 5, 0], [53.5, 5, 0], [20.5, 5, 0], [119, 5, 0]];

difference()
{
  union()
  {
    // Plate bottom & border
    cube([$boardwidth, $boarddepth, $platethick]);
    translate([0, 0, $platethick])
    {
      cube([$boardwidth, $borderwidth, $supportheight]);
    }
    translate([0, $boarddepth-$borderwidth, $platethick])
    {
      cube([$boardwidth, $borderwidth, $supportheight]);
    }
    translate([0, 0, $platethick])
    {
      cube([$borderwidth, $boarddepth, $supportheight]);
    }
    translate([$boardwidth-$borderwidth, 0, $platethick])
    {
      cube([$borderwidth, $boarddepth, $supportheight]);
    }
    
    // #2 screw supports (no holes yet)
    for ($sc2l = $screw2locs)
    {
      translate($sc2l)
      {
        translate([0, 0, $platethick])
        {
          cylinder(r=0.5*$screw2supportdia, h=$supportheight);
        }
      }
    }
    
    // #4 screw supports (no holes yet)
    for ($sc4l = $screw4locs)
    {
      translate($sc4l)
      {
        translate([0, 0, $platethick])
        {
          cylinder(r=0.5*$screw4supportdia, h=$supportheight);
        }
      }
    }
    
    // bosses
    for ($bl = $bosslocs)
    {
      translate($bl)
      {
        translate([0, 0, $platethick])
        {
          cylinder(r=0.5*$bosssupoortdia, h=$supportheight);
        }
        translate([0, 0, $platethick + $supportheight])
        {
          cylinder(r=0.5*$bosspostdia, h=$bosspostheight);
        }
      }
    }
  }
  
  // #2 screw holes
  for ($sc2l = $screw2locs)
  {
    translate($sc2l)
    {
      translate([0, 0, $platethick + $supportheight - $screw2holedepth])
      {
        cylinder(r=0.5*$screw2holedia, h=$screw2holedepth);
      }
    }
  }
  
  // #4 screw holes
  for ($sc4l = $screw4locs)
  {
    translate($sc4l)
    {
      translate([0, 0, $platethick + $supportheight - $screw4holedepth])
      {
        cylinder(r=0.5*$screw4holedia, h=$screw4holedepth);
      }
    }
  }
}
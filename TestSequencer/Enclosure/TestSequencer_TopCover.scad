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
$wallthick = 2.94;
$screw4holedia = 3.3;
$screwxinset = 9;
$screwyinset = 3.5;
$totalwidth = 109;
$totaldepth = 53;
$innerheight = 12;
$flangedepth = 7;

$screw4locs = [[$screwxinset,$screwyinset,0], [$totalwidth-$screwxinset,$screwyinset,0], [$screwxinset,$totaldepth-$screwyinset,0], [$totalwidth-$screwxinset,$totaldepth-$screwyinset,0]];

difference()
{
  union()
  {
    cube([$totalwidth, $flangedepth+$wallthick, $wallthick]);
    translate([0, $totaldepth-$flangedepth-$wallthick, 0])
    {
      cube([$totalwidth, $flangedepth+$wallthick, $wallthick]);
    }
    translate([0, $flangedepth, 0])
    {
      cube([$totalwidth, $wallthick, $innerheight+$wallthick]);
    }
    translate([0, $totaldepth-$flangedepth-$wallthick, 0])
    {
      cube([$totalwidth, $wallthick, $innerheight+$wallthick]);
    }
    translate([0, $flangedepth, $innerheight])
    {
      cube([$totalwidth, $totaldepth - 2*$flangedepth, $wallthick]);
    }
    rotate([90, 0, 90])
    {
      linear_extrude(height=$wallthick)
      {
        polygon(points = [[0,0], [0,$wallthick], [$flangedepth, $innerheight+$wallthick], [$totaldepth-$flangedepth,$innerheight+$wallthick], [$totaldepth, $wallthick], [$totaldepth, 0]]);
      }
    }
    translate([$totalwidth-$wallthick, 0, 0])
    {
      rotate([90, 0, 90])
      {
        linear_extrude(height=$wallthick)
        {
          polygon(points = [[0,0], [0,$wallthick], [$flangedepth, $innerheight+$wallthick], [$totaldepth-$flangedepth,$innerheight+$wallthick], [$totaldepth, $wallthick], [$totaldepth, 0]]);
        }
      }
    }
  }
  
  
  for ($s4l = $screw4locs)
  {
    translate($s4l)
    {
      cylinder(r=0.5*$screw4holedia, h=$wallthick);
    }
  }
}
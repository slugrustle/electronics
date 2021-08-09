// This file is meant for use with OpenSCAD, a
// constructive solid geometry 3D CAD program.
// See https://www.openscad.org/
$fa = 6;
$fs = 0.05;

// Dimensional constants. Lengths are in millimeters.
// Some dimensions are rounded to 0.14mm layer height
platethick = 4.06;
bossheight = 6.16;
boss_outer_radius = 3.75;
screw_hole_radius = 1.3;
screw_hole_depth = 8.12;
ribthick = 3;
riblength = 3;
ribheight = 4.76;
ribchamfer = 0.98;
supportheight = bossheight;
supportinset = 6;
supportthick = 4;
notchheight = 3;
notchlength = 3;
supportgap = 0.5;
plateborder = 10;
boardwidth = 165;
boardlength = 160;

// Screw boss locations and rib selections
// [[X, Y], [N, S, E, W]]
// First 2 are screw boss center locations on board in millimeters
// Last 4 are ribs to enable (true) or disable (false)
screwbosses = [
  [[7.65, 145.9], [true, true, true, false]], 
  [[47.15, 145.9], [true, true, true, true]],
  [[37.4, 86.9], [true, true, true, true]],
  [[37.4, 67.1], [true, true, true, true]],
  [[81.9, 6.5], [true, false, true, true]],
  [[129.6, 80.7], [true, true, true, true]],
  [[84.7, 36.025], [true, true, true, true]],
  [[84.7, 93.525], [true, true, false, true]],
  [[93.175, 143.9], [true, false, true, true]],
  [[150.675, 143.9], [true, true, true, true]]
];

// Board north side support locations
// millimeters east of northwest board corner
northsupports = [27.4, 68];

// Board south side support locations
// millimeters east of southwest board corner
southsupports = [40, 97.3, 127.5, 153];

// Board east side support locations
// millimeters north of southeast board corner
eastsupports = [19, 41.7, 69.2, 86.3, 112];

// Board west side support locations
// millimeters north of southwest board corner
westsupports = [15, 45, 102.5];

module screwboss(boardloc2d, ribenables)
{
  module bossrib()
  {
    translate([0,ribthick/2,0])
    {
      rotate([90,0,0])
      {
        linear_extrude(height=ribthick)
        {
          polygon(points = [[0,0], [riblength+0.2,0],[riblength+0.2,ribheight-ribchamfer],[riblength+0.2-ribchamfer,ribheight],[0,ribheight]]);
        }
      }
    }
  }
  
  union()
  {
    translate(concat(boardloc2d, [0]) + [plateborder,plateborder,platethick])
    {
      cylinder(r=boss_outer_radius, h=bossheight);
    }
    
    if (ribenables[0])
    {
      translate(concat(boardloc2d, [0]) + [plateborder,plateborder,platethick] + [0, boss_outer_radius-0.2, 0])
      {
        rotate([0,0,90])
        {
          bossrib();
        }
      }
    }
    
    if (ribenables[1])
    {
      translate(concat(boardloc2d, [0]) + [plateborder,plateborder,platethick] + [0, -boss_outer_radius+0.2, 0])
      {
        rotate([0,0,-90])
        {
          bossrib();
        }
      }
    }
    
    if (ribenables[2])
    {
      translate(concat(boardloc2d, [0]) + [plateborder,plateborder,platethick] + [boss_outer_radius-0.2, 0, 0])
      {
        bossrib();
      }
    }
    
    if (ribenables[3])
    {
      translate(concat(boardloc2d, [0]) + [plateborder,plateborder,platethick] + [-boss_outer_radius+0.2, 0, 0])
      {
        rotate([0,0,180])
        {
          bossrib();
        }
      }
    }
  }
}

module boardsupport()
{
  // Default orientation is for an east support centered at 0 along  a fictitious board edge formed by the Y axis
  translate([-supportinset,supportthick/2,0])
  {
    rotate([90, 0, 0])
    {
      linear_extrude(height = supportthick)
      {
        polygon(points = [[0,0], [supportinset+supportgap+notchlength,0], [supportinset+supportgap+notchlength,supportheight+notchheight], [supportinset+supportgap,supportheight+notchheight], [supportinset+supportgap,supportheight], [0,supportheight]]);
      }
    }
  }
}

// Put everything together
difference()
{
  union()
  {
    // Bottom plate
    cube([2*plateborder+boardwidth, 2*plateborder+boardlength, platethick]);
    
    // Screw bosses but not screw holes
    for(sb = screwbosses)
    {
      screwboss(sb[0], sb[1]);
    }
    
    // North side board supports
    for (ns = northsupports)
    {
      translate([plateborder + ns, plateborder + boardlength, platethick])
      {
        rotate([0, 0, 90])
        {
          boardsupport();
        }
      }
    }
    
    // South side board supports
    for (ss = southsupports)
    {
      translate([plateborder + ss, plateborder, platethick])
      {
        rotate([0, 0, -90])
        {
          boardsupport();
        }
      }
    }
    
    // East side board supports
    for (es = eastsupports)
    {
      translate([plateborder + boardwidth, plateborder + es, platethick])
      {
        boardsupport();
      }
    }
    
    // West side board supports
    for (ws = westsupports)
    {
      translate([plateborder, plateborder + ws, platethick])
      {
        rotate([0, 0, 180])
        {
          boardsupport();
        }
      }
    }
    
  }
  
  // Holes for screws
  for(sb = screwbosses)
  {
    translate([plateborder + sb[0][0], plateborder + sb[0][1], platethick + bossheight - screw_hole_depth])
    {
      cylinder(r=screw_hole_radius, h=screw_hole_depth);
    }
  }
}

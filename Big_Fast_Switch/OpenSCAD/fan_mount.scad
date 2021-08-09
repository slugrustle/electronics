$fa = 6;
$fs = 0.05;

$wallthick = 3.08;
$totalwidth = 67.5;
$cornerside = 6*sqrt(2);
$fanside = 41.5;
$totalheight = $fanside+2*$wallthick;
$bendheight = 13+$wallthick;
$bendwidth = 12.88;
$bendgap = 0.98;
$shortvertheight = 6+$wallthick;
$betweenplates = 21.56;
$screwholedia = 3.5;
$screwholecenterinset = 5;
$sidesupport = 2*$screwholecenterinset;

module face_plate()
{
  linear_extrude($wallthick)
  {
    polygon(points=[[0,0],[0,$wallthick],[$sidesupport,$totalheight],[$totalheight+$sidesupport,$totalheight],[$totalwidth,$wallthick],[$totalwidth,0],[$sidesupport+$wallthick+$cornerside,$wallthick],[$sidesupport+$wallthick,$wallthick+$cornerside],[$sidesupport+$wallthick,$fanside+$wallthick-$cornerside],[$sidesupport+$wallthick+$cornerside,$fanside+$wallthick],[$fanside+$sidesupport+$wallthick-$cornerside,$fanside+$wallthick],[$fanside+$sidesupport+$wallthick,$fanside+$wallthick-$cornerside],[$fanside+$sidesupport+$wallthick,$wallthick+$cornerside],[$fanside+$sidesupport+$wallthick-$cornerside,$wallthick]], paths=[[0,1,2,3,4,5],[6,7,8,9,10,11,12,13]]);
  }
}

module bend()
{
  linear_extrude($bendwidth)
  {
    polygon(points=[[-2.5,-2*sqrt(6)],[0,0],[$wallthick/2,0],[-2.5+$wallthick*2*sqrt(6)/11,-2*sqrt(6)-$wallthick*5/22]]);
  }
}

difference()
{
  union()
  {
    // bottom plate
    cube([$totalwidth,2*$wallthick+$betweenplates,$wallthick]);
    
    // left lower side plate
    translate([$sidesupport,0,0])
    {
      cube([$wallthick,2*$wallthick+$betweenplates,$shortvertheight]);
    }
    
    // left bend sides
    translate([$sidesupport,0,0])
    {
      cube([$wallthick,$wallthick+($betweenplates-$bendwidth-2*$bendgap)/2,$wallthick+$bendheight+0.001]);
    }
    
    translate([$sidesupport,$wallthick+$betweenplates-($betweenplates-$bendwidth-2*$bendgap)/2,0])
    {
      cube([$wallthick,$wallthick+($betweenplates-$bendwidth-2*$bendgap)/2,$wallthick+$bendheight+0.001]);
    }
    
    // left upper side plate
    translate([$sidesupport,0,$bendheight])
    {
      cube([$wallthick,2*$wallthick+$betweenplates,$totalheight-$bendheight]);
    }
    
    // leftt bend
    translate([$sidesupport+$wallthick,(2*$wallthick+$betweenplates-$bendwidth)/2,$bendheight])
    {
      rotate([0,0,180])
      {
        rotate([90,0,0])
        {
          bend();
        }
      }
    }
    
    // right lower side plate
    translate([$fanside+$sidesupport+$wallthick,0,0])
    {
      cube([$wallthick,2*$wallthick+$betweenplates,$shortvertheight]);
    }
    
    // right bend sides
    translate([$fanside+$sidesupport+$wallthick,0,0])
    {
      cube([$wallthick,$wallthick+($betweenplates-$bendwidth-2*$bendgap)/2,$wallthick+$bendheight+0.001]);
    }
    
    translate([$fanside+$sidesupport+$wallthick,$wallthick+$betweenplates-($betweenplates-$bendwidth-2*$bendgap)/2,0])
    {
      cube([$wallthick,$wallthick+($betweenplates-$bendwidth-2*$bendgap)/2,$wallthick+$bendheight+0.001]);
    }
    
    // right upper side plate
    translate([$fanside+$sidesupport+$wallthick,0,$bendheight])
    {
      cube([$wallthick,2*$wallthick+$betweenplates,$totalheight-$bendheight]);
    }
    
    // right bend
    translate([$fanside+$sidesupport+$wallthick,(2*$wallthick+$betweenplates+$bendwidth)/2,$bendheight])
    {
      rotate([90,0,0])
      {
        bend();
      }
    }
    
    // rear face
    translate([0,$wallthick,0])
    {
      rotate([90,0,0])
      {
        face_plate();
      }
    }

    // front face
    translate([0,2*$wallthick+$betweenplates,0])
    {
      rotate([90,0,0])
      {
        face_plate();
      }
    }
  }

  // screw holes
  translate([$screwholecenterinset,(2*$wallthick+$betweenplates)/2,0])
  {
    cylinder(r=$screwholedia/2,h=$wallthick);
  }
  
  translate([$totalwidth-$screwholecenterinset,(2*$wallthick+$betweenplates)/2,0])
  {
    cylinder(r=$screwholedia/2,h=$wallthick);
  }  
}
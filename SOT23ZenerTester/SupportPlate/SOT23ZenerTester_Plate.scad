// This file is meant for use with OpenSCAD, a
// constructive solid geometry 3D CAD program.
// See https://www.openscad.org/
// The height dimensions have been rounded to best
// support a 0.14mm layer height.
$fa = 3;
$fs = 0.05;

$platethick = 4.34;
$screwblind = 0.7;
$standoffheight = 2.94;
$standoff_inner_radius = 0.8;
$standoff_outer_radius = 2.9;
$boardwidth = 135;
$boarddepth = 108;
$borderwidth = 15;

$S1_boardx = 8.5;
$S1_boardy = 37;
$S2_boardx = 8.5;
$S2_boardy = 83.5;
$S3_boardx = 31.5;
$S3_boardy = 6;
$S4_boardx = 52.5;
$S4_boardy = 6;
$S5_boardx = 73.5;
$S5_boardy = 6;
$S6_boardx = 94.5;
$S6_boardy = 6;
$S7_boardx = 115.5;
$S7_boardy = 6;
$S8_boardx = 42;
$S8_boardy = 47.5;
$S9_boardx = 63;
$S9_boardy = 47.5;
$S10_boardx = 84;
$S10_boardy = 47.5;
$S11_boardx = 105;
$S11_boardy = 47.5;
$S12_boardx = 126;
$S12_boardy = 47.5;
$S13_boardx = 31.5;
$S13_boardy = 104;
$S14_boardx = 46.5;
$S14_boardy = 104;
$S15_boardx = 129.5;
$S15_boardy = 73;
$S16_boardx = 129.5;
$S16_boardy = 95.5;
$S17_boardx = 110.5;
$S17_boardy = 103.5;
$S18_boardx = 79;
$S18_boardy = 97.5;

$S1x = $borderwidth + $S1_boardx;
$S1y = $borderwidth + $S1_boardy;
$S2x = $borderwidth + $S2_boardx;
$S2y = $borderwidth + $S2_boardy;
$S3x = $borderwidth + $S3_boardx;
$S3y = $borderwidth + $S3_boardy;
$S4x = $borderwidth + $S4_boardx;
$S4y = $borderwidth + $S4_boardy;
$S5x = $borderwidth + $S5_boardx;
$S5y = $borderwidth + $S5_boardy;
$S6x = $borderwidth + $S6_boardx;
$S6y = $borderwidth + $S6_boardy;
$S7x = $borderwidth + $S7_boardx;
$S7y = $borderwidth + $S7_boardy;
$S8x = $borderwidth + $S8_boardx;
$S8y = $borderwidth + $S8_boardy;
$S9x = $borderwidth + $S9_boardx;
$S9y = $borderwidth + $S9_boardy;
$S10x = $borderwidth + $S10_boardx;
$S10y = $borderwidth + $S10_boardy;
$S11x = $borderwidth + $S11_boardx;
$S11y = $borderwidth + $S11_boardy;
$S12x = $borderwidth + $S12_boardx;
$S12y = $borderwidth + $S12_boardy;
$S13x = $borderwidth + $S13_boardx;
$S13y = $borderwidth + $S13_boardy;
$S14x = $borderwidth + $S14_boardx;
$S14y = $borderwidth + $S14_boardy;
$S15x = $borderwidth + $S15_boardx;
$S15y = $borderwidth + $S15_boardy;
$S16x = $borderwidth + $S16_boardx;
$S16y = $borderwidth + $S16_boardy;
$S17x = $borderwidth + $S17_boardx;
$S17y = $borderwidth + $S17_boardy;
$S18x = $borderwidth + $S18_boardx;
$S18y = $borderwidth + $S18_boardy;

difference()
{
  union()
  {
    // Flat part of plate
    cube([$boardwidth+2*$borderwidth,$boarddepth+2*$borderwidth,$platethick]);
    
    // Board standoffs
    translate([$S1x, $S1y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S2x, $S2y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S3x, $S3y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S4x, $S4y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S5x, $S5y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S6x, $S6y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S7x, $S7y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S8x, $S8y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S9x, $S9y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S10x, $S10y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S11x, $S11y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S12x, $S12y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S13x, $S13y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S14x, $S14y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S15x, $S15y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S16x, $S16y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S17x, $S17y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
    
    translate([$S18x, $S18y, $platethick])
    {
      cylinder(r=$standoff_outer_radius, h=$standoffheight);
    }
  }
  
  // Standoff screw holes
  translate([$S1x, $S1y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S2x, $S2y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S3x, $S3y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S4x, $S4y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S5x, $S5y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S6x, $S6y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S7x, $S7y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S8x, $S8y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S9x, $S9y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S10x, $S10y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S11x, $S11y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S12x, $S12y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S13x, $S13y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S14x, $S14y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S15x, $S15y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S16x, $S16y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S17x, $S17y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
  translate([$S18x, $S18y, $screwblind])
  {
    cylinder(r=$standoff_inner_radius, h=$platethick+$standoffheight-$screwblind);
  }
  
}
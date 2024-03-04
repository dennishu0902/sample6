#include "seam_carver.hpp"
//#include <limits>
// implement the rest of SeamCarver's functions here

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}
//start here
// returns the instance's image_
const ImagePPM& SeamCarver::GetImage() const
{
    return image_ ;
}

// returns the image's height
int SeamCarver::GetHeight() const
{
   return height_;
}

// returns the image's width
int SeamCarver::GetWidth() const
{
   return width_ ;
}

// returns the energy of the pixel at row col in image_
//    | A |   |
// ---+---+---+---
//  C |   | D |
// ---+---+---+---
//    | B |   | 
int SeamCarver::GetEnergy(int row, int col) const
{
   int row_0=0, row_2=0;
   int col_0=0, col_2=0;
   int Energy = 0;

   if(row == 0) row_0 = height_ -1;
   else row_0 = row - 1;

   if(row == (height_ - 1)) row_2 = 0;
   else row_2 = row + 1;

   if(col == 0) col_0 = width_ - 1;
   else col_0 = col - 1;

   if(col == (width_ - 1)) col_2 = 0;
   else col_2 = col + 1;

   Pixel a =  image_.GetPixel(row_0,col);
   Pixel b =  image_.GetPixel(row_2,col);
   Pixel c =  image_.GetPixel(row,col_0);
   Pixel d =  image_.GetPixel(row,col_2);
 
   Energy += (a.GetRed()-b.GetRed())*(a.GetRed()-b.GetRed());
   Energy += (a.GetGreen()-b.GetGreen())*(a.GetGreen()-b.GetGreen());
   Energy += (a.GetBlue()-b.GetBlue())*(a.GetBlue()-b.GetBlue());
   Energy += (c.GetRed()-d.GetRed())*(c.GetRed()-d.GetRed());
   Energy += (c.GetGreen()-d.GetGreen())*(c.GetGreen()-d.GetGreen());
   Energy += (c.GetBlue()-d.GetBlue())*(c.GetBlue()-d.GetBlue());
   return Energy;

}

  // returns the horizontal seam of image_ with the least amount of
  // energy
  //
  // the ith int in the returned array corresponds to which row at
  // col i is in the seam. example:
  //
  //    | x |   |
  // ---+---+---+---
  //  x |   | x |
  // ---+---+---+---
  //    |   |   | x
  // returns {1, 0, 1, 2}
int* SeamCarver::GetHorizontalSeam()
{
     int *seam_curve;
     seam_curve = new int[width_];
     InitMemo();
     FindmMinPathMemoDynamic(false);//calculate Seam
     //Seam Curve
     int min_seam= GetRowColinMemo(0,0); 
     int min_row = 0;
     for(int row=1;row<height_;row++)
        if( GetRowColinMemo(row,0) < min_seam)
           {
            min_seam = GetRowColinMemo(row,0);
            min_row  = row; 
           } 

     seam_curve[0] = min_row;
     int row = min_row;
     for(int col=1; col < width_; col++)
       {
        row = min_row;
        min_seam = GetRowColinMemo(row, col);//middle
        if(GetRowColinMemo(row-1,col) < min_seam) //up
          { min_row = row - 1;
            min_seam = GetRowColinMemo(row-1, col);
           }
        if(GetRowColinMemo(row+1, col) < min_seam) //down
          { min_row = row +1;
           }
        seam_curve[col] = min_row;   
       }  
       ClearMemo();
       return seam_curve;
}

  // returns the vertical seam of image_ with the least amount of
  // energy
  //
  // the ith int in the returned array corresponds to which col at
  // row i is in the seam. example:
  //
  //    | x |   |
  // ---+---+---+---
  //    |   | x |
  // ---+---+---+---
  //    |   | x |
  // returns {1, 2, 2}
int* SeamCarver::GetVerticalSeam() 
{
     int *seam_curve;
     seam_curve = new int[height_];
     InitMemo();
     FindmMinPathMemoDynamic(true);//calculate Seam
     //Seam Curve
     int min_seam= GetRowColinMemo(0,0); 
     int min_col = 0;
     for(int col=1;col<width_;col++)
        if( GetRowColinMemo(0,col) < min_seam)
           {
            min_seam = GetRowColinMemo(0,col);
            min_col  = col; 
           } 
     
     seam_curve[0] = min_col;
     int col = min_col;
     for(int row=1; row < height_ ; row++)
       {
        col = min_col;
        min_seam = GetRowColinMemo(row, col);//middle
        if(GetRowColinMemo(row, col-1) < min_seam) //left
          { min_col = col - 1;
            min_seam = GetRowColinMemo(row, col-1);
           }
        if(GetRowColinMemo(row, col+1) < min_seam) //right
           min_col = col +1;     
        seam_curve[row] = min_col; 
       }
       ClearMemo(); 
       return seam_curve;
}
  // removes one horizontal seam in image_. example:
  //
  // image_ before:
  //  0 | 1 | 2 | 3
  // ---+---+---+---
  //  4 | 5 | 6 | 7
  // ---+---+---+---
  //  8 | 9 | 10| 11
  //
  // seam to remove:
  //    | x |   |
  // ---+---+---+---
  //  x |   | x |
  // ---+---+---+---
  //    |   |   | x
  //
  // image_ after:
  //  0 | 5 | 2 | 3
  // ---+---+---+---
  //  8 | 9 | 10| 7
void SeamCarver::RemoveHorizontalSeam()
{
    int* pSeamCurve = GetHorizontalSeam();
    int  row = 0;
    
    //Remove Image by Seam Curve
    for(int col=0;col<width_;col++)
    {
      row = pSeamCurve[col];
      while(row < height_ -1)
         {  Pixel px = image_.GetPixel(row+1,col);
            image_.PutPixel(row, col,px);
            row++;
         }
    }
    delete pSeamCurve;
    height_--;
    image_.SetHeight(height_);

}

  // removes one vertical seam in image_. example:
  //
  // image_ before:
  //  0 | 1 | 2 | 3
  // ---+---+---+---
  //  4 | 5 | 6 | 7
  // ---+---+---+---
  //  8 | 9 | 10| 11
  //
  // seam to remove:
  //    | x |   |
  // ---+---+---+---
  //    |   | x |
  // ---+---+---+---
  //    |   | x |
  //
  // image_ after:
  //  0 | 2 | 3
  // ---+---+---
  //  4 | 5 | 7
  // ---+---+---
  //  8 | 9 | 11
void SeamCarver::RemoveVerticalSeam()
{
   
   int* pSeamCurve = GetVerticalSeam();
   int  col = 0;
   
  //Remove Image by Seam Curve
   for(int row=0;row<height_;row++)
    {
      col = pSeamCurve[row];
      while(col < width_ -1)
         {
            Pixel px = image_.GetPixel(row,col+1);
            image_.PutPixel(row, col,px);
            col++;
            }
    }
    delete pSeamCurve;
    width_--;
    image_.SetWidth(width_);
}
void SeamCarver::InitMemo()
{
   int int_max = std::numeric_limits<int>::max();
   std::vector<int> v1;
   v1.resize(width_, int_max);
   memo_.resize(height_,v1);
}
void SeamCarver::ClearMemo()
{
  memo_.clear();
}
int SeamCarver::GetRowColinMemo(int row,int col) const
{
  // int int_max = std::numeric_limits<int>::max();
   if(row < 0) row = 0;
   if(row > height_ - 1) row = height_ -1;
   if(col < 0) col = 0;
   if(col > width_ - 1) col = width_ -1;
   return memo_[row][col];
  /*  if( row >= 0 && row < height_ && col >=0 && col < width_ )
       return memo_[row][col];
   else
       return int_max;    */   
 }
void SeamCarver::PutRowColinMemo(int row,int col, int beste)
{
   memo_[row][col] = beste;
}

void SeamCarver::FindmMinPathMemoDynamic(bool isVertical) 
{
  int row=0;
  int col=0;

  if(isVertical) //
  {
  row =  height_ -1; //last row
  for(col=0;col<width_;col++)
       PutRowColinMemo(row, col,GetEnergy(row,col));
  for(row=height_ -2; row>-1; row--)
     for(col=0; col < width_ ; col++)
      PutRowColinMemo(row, col,GetEnergy(row,col) +
        min(min(GetRowColinMemo(row+1,col-1),GetRowColinMemo(row+1,col+1)),GetRowColinMemo(row+1,col)));
    }
  else
  {
  col =  width_ -1; //last col
  for(row=0;row<height_;row++)
     PutRowColinMemo(row, col,GetEnergy(row,col));
  for(col=width_ -2; col>-1; col--)
    {
      for(row=0; row <height_ ; row++)
       PutRowColinMemo(row, col,GetEnergy(row,col) +
        min(min(GetRowColinMemo(row-1,col+1),GetRowColinMemo(row+1,col+1)),GetRowColinMemo(row,col+1)));
    }
  }
}

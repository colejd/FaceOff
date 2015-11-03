//
//  ParallelContourDetector.hpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/29/15.
//
//

#ifndef ParallelContourDetector_hpp
#define ParallelContourDetector_hpp

#include "opencv2/opencv.hpp"

/**
 * Takes an image, divides it up, and processes it in separate threads using TBB.
 */
class ParallelContourDetector : public cv::ParallelLoopBody {
private:
    cv::UMat input;
    cv::UMat output;
    int subsections;
    int lineThickness;
    
public:
    
    ParallelContourDetector(cv::UMat& _src_gray, cv::UMat& _out_gray, int _subsections, int _lineThickness);
    
    virtual void operator ()(const cv::Range &range) const;
    
    static void DetectContoursParallel(cv::UMat& in, cv::UMat& out, const int subsections, const int lineThickness);
    
    
};

#endif /* ParallelContourDetector_hpp */

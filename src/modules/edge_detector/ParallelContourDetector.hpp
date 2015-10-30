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
 * Takes an image and processes it in segments, which are distributed across the CPU cores by TBB.
 */
class ParallelContourDetector : public cv::ParallelLoopBody {
private:
    cv::Mat src_gray;
    cv::Mat &out_gray;
    int subsections;
    int lineThickness;
    
public:
    
    ParallelContourDetector(cv::Mat _src_gray, cv::Mat &_out_gray, int _subsections, int _lineThickness) : src_gray(_src_gray), out_gray(_out_gray), subsections(_subsections), lineThickness(_lineThickness) {};
    ~ParallelContourDetector();
    
    virtual void operator ()(const cv::Range &range) const;
    
    
};

#endif /* ParallelContourDetector_hpp */

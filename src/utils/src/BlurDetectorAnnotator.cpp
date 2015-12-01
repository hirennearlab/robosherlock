/* Copyright (c) 2013, Thiemo Wiedemeyer  <wiedemeyer@informatik.uni-bremen.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Intelligent Autonomous Systems Group/
 *       Technische Universitaet Muenchen nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// STL
#include <map>

// UIMA
#include <uima/api.hpp>

// OpenCV
#include <opencv2/opencv.hpp>

// RS
#include <rs/scene_cas.h>
#include <rs/utils/time.h>
#include <rs/utils/output.h>
#include <rs/DrawingAnnotator.h>
#include <rs/utils/BlurDetector.h>

using namespace uima;

class BlurDetectorAnnotator : public DrawingAnnotator
{
private:
  BlurDetector detector;
  cv::Mat color, grey;

  typedef double(*function)(const cv::Mat &img);
  std::vector<function> functions;
  std::vector<std::string> names;
  std::vector<std::vector<double>> results;
  std::vector<double> timing;
  std::vector<bool> isBlurred;
  size_t algorithms;
  size_t useAlgorithm;
  size_t frames;

  const bool justPlot;

public:
  BlurDetectorAnnotator() : DrawingAnnotator(__func__), justPlot(true)
  {
    frames = 0;

    //names.push_back("LaplaceSum");
    //functions.push_back(&BlurDetector::funcLaplaceSum);

    //names.push_back("LaplaceMean");
    //functions.push_back(&BlurDetector::funcLaplaceMean);

    //names.push_back("LaplaceStdDev");
    //functions.push_back(&BlurDetector::funcLaplaceStdDev);

    //names.push_back("SobelSum");
    //functions.push_back(&BlurDetector::funcSobelSum);

    //names.push_back("SobelMean");
    //functions.push_back(&BlurDetector::funcSobelMean);

    names.push_back("SobelStdDev");
    functions.push_back(&BlurDetector::funcSobelStdDev);

    useAlgorithm = names.size();
    names.push_back("SobelStdDevOptimized");
    functions.push_back(&BlurDetector::funcSobelStdDevOptimized);

    //names.push_back("ModifiedLaplace");
    //functions.push_back(&BlurDetector::funcModifiedLaplace);

    //names.push_back("Tenengrad");
    //functions.push_back(&BlurDetector::funcTenengrad);

    //names.push_back("GraylevelVariance");
    //functions.push_back(&BlurDetector::funcNormalizedGraylevelVariance);

    algorithms = names.size();
    results.resize(algorithms);
    timing.resize(algorithms, 0);
  }

  /*
   * Initializes annotator
   */
  TyErrorId initialize(AnnotatorContext &ctx)
  {
    outInfo("initialize");
    return UIMA_ERR_NONE;
  }

  /*
   * Destroys annotator
   */
  TyErrorId destroy()
  {
    outInfo("destroy");
    return UIMA_ERR_NONE;
  }

private:
  /*
   * Processes a frame
   */
  TyErrorId processWithLock(CAS &tcas, ResultSpecification const &res_spec)
  {
    if(justPlot)
    {
      return UIMA_ERR_NONE;
    }

    MEASURE_TIME;
    outInfo("process begins");
    rs::SceneCas cas(tcas);

    cas.get(VIEW_COLOR_IMAGE_HD, color);
    cv::cvtColor(color, grey, CV_BGR2GRAY);

    ros::Time start, end;
    double result;
    for(size_t i = 0; i < algorithms; ++i)
    {
      const function &func = functions[i];

      start = ros::Time::now();
      result = (*func)(grey);
      end = ros::Time::now();
      timing[i] += (end - start).toNSec() / 1000000.0;

      outDebug(names[i] << ": " << result << " (" << (end - start).toNSec() / 1000000.0 << " ms)");

      results[i].push_back(result);
    }
    ++frames;

    isBlurred.push_back(detector.detectBlur(results[useAlgorithm].back()));

    return UIMA_ERR_NONE;
  }

  void drawImageWithLock(cv::Mat &disp)
  {
    if(justPlot)
    {
      plotResultsGlobal(disp);
    }
    else
    {
      plotResults(disp);
    }
  }

  void plotResults(cv::Mat &disp)
  {
    const int height = 1900;
    const int width = 1000;
    const int text = 20;
    disp = cv::Mat::zeros(height + text * algorithms + 1, width, CV_8UC3);

    if(frames < 2)
    {
      return;
    }

    const std::vector<cv::Scalar> colors = { CV_RGB(255, 0, 0),
                                             CV_RGB(0, 255, 0),
                                             CV_RGB(0, 0, 255),
                                             CV_RGB(255, 255, 0),
                                             CV_RGB(255, 0, 255),
                                             CV_RGB(0, 255, 255),
                                             CV_RGB(127, 0, 0),
                                             CV_RGB(0, 127, 0),
                                             CV_RGB(0, 0, 127),
                                             CV_RGB(127, 127, 0),
                                             CV_RGB(127, 0, 127),
                                             CV_RGB(0, 127, 127),
                                             CV_RGB(255, 127, 0),
                                             CV_RGB(255, 0, 127),
                                             CV_RGB(0, 255, 127),
                                             CV_RGB(127, 255, 0),
                                             CV_RGB(127, 0, 255),
                                             CV_RGB(0, 127, 255)
                                           };

    std::vector<std::vector<cv::Point>> lines(algorithms, std::vector<cv::Point>(frames));

    for(size_t i = 0; i < algorithms; ++i)
    {
      const std::string &name = names[i];
      const std::vector<double> &results = this->results[i];
      double min = results[0];
      double max = results[0];

      for(size_t j = 1; j < frames; ++j)
      {
        min = std::min(results[j], min);
        max = std::max(results[j], max);
      }
      max -= min;

      std::vector<cv::Point> &line = lines[i];
      for(size_t j = 0; j < frames; ++j)
      {
        double norm = (results[j] - min) / max;
        line[j].x = (int)(((j * width) / (double)frames) + 0.5);
        line[j].y = (int)(height * (1.0 - norm) + 0.5);
      }

      std::ostringstream oss;
      oss << name << " (" << timing[i] / frames << " ms)";
      cv::Point pos(5, height + text * (i + 1) - 5);
      cv::putText(disp, oss.str(), pos, cv::FONT_HERSHEY_SIMPLEX, 0.5, colors[i % colors.size()], 1, CV_AA);
    }

    for(size_t i = 0; i < frames; ++i)
    {
      int x0 = (int)(((i * width) / (double)frames) + 0.5);
      int x1 = (int)((((i + 1) * width) / (double)frames) - 0.5);
      if(isBlurred[i])
      {
        cv::rectangle(disp, cv::Point(x0, 0), cv::Point(x1, height), CV_RGB(63, 63, 63), CV_FILLED);
      }
    }

    for(size_t i = 10; i < frames; i += 10)
    {
      int x = (int)(((i * width) / (double)frames) + 0.5);
      cv::line(disp, cv::Point(x, 0), cv::Point(x, height), CV_RGB(127, 127, 127), 1, CV_AA);
    }

    for(size_t i = 0; i < algorithms; ++i)
    {
      std::vector<cv::Point> &line = lines[i];

      cv::Point p0, p1;
      p0 = line[0];
      for(size_t j = 1; j < frames; ++j)
      {
        p1 = line[j];
        cv::line(disp, p0, p1, colors[i % colors.size()], 1, CV_AA);
        p0 = p1;
      }
    }
  }

  void plotResultsGlobal(cv::Mat &disp)
  {
    const int height = 800;
    const int width = 1600;
    disp = cv::Mat::zeros(height, width, CV_8UC3);

    const size_t frames = BlurDetector::results.size();

    if(frames < 2)
    {
      return;
    }

    std::vector<cv::Point> line(frames);

    std::list<double>::const_iterator it = BlurDetector::results.begin();
    double min = *it;
    double max = *it;

    ++it;
    for(size_t i = 1; i < frames; ++i, ++it)
    {
      min = std::min(*it, min);
      max = std::max(*it, max);
    }
    max -= min;

    it = BlurDetector::results.begin();
    for(size_t i = 0; i < frames; ++i, ++it)
    {
      double norm = (*it - min) / max;
      line[i].x = (int)(((i * width) / (double)frames) + 0.5);
      line[i].y = (int)(height * (1.0 - norm) + 0.5);
    }

    std::list<bool>::const_iterator itB = BlurDetector::isBlurred.begin();
    for(size_t i = 0; i < frames; ++i, ++itB)
    {
      int x0 = (int)(((i * width) / (double)frames) + 0.5);
      int x1 = (int)((((i + 1) * width) / (double)frames) - 0.5);
      if(*itB)
      {
        cv::rectangle(disp, cv::Point(x0, 0), cv::Point(x1, height), CV_RGB(63, 63, 63), CV_FILLED);
      }
    }

    for(size_t i = 10; i < frames; i += 10)
    {
      int x = (int)(((i * width) / (double)frames) + 0.5);
      cv::line(disp, cv::Point(x, 0), cv::Point(x, height), CV_RGB(127, 127, 127), 1, CV_AA);
    }

    cv::Point p0, p1;
    p0 = line[0];
    for(size_t i = 1; i < frames; ++i)
    {
      p1 = line[i];
      cv::line(disp, p0, p1, CV_RGB(255, 0, 0), 1, CV_AA);
      p0 = p1;
    }
  }
};

MAKE_AE(BlurDetectorAnnotator)
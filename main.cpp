#include <Vector>
#include <Windows.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using boost::posix_time::ptime;
using boost::posix_time::second_clock;
using boost::posix_time::to_simple_string;
using boost::gregorian::day_clock;

class myTime {
public:
  ptime getCurrentTime() { return ptime(day_clock::universal_day(), second_clock::universal_time().time_of_day()); }

  void setStartDuration(int index) { mduration_start_time[index] = boost::posix_time::microsec_clock::local_time(); }

  boost::posix_time::time_duration getResultDuration(int index) {
    boost::posix_time::time_duration dur = boost::posix_time::microsec_clock::local_time() - mduration_start_time[index];
    mduration_start_time.erase(index);
    return  dur;
  }

  long getDurdationMs(const boost::posix_time::time_duration dur) { return dur.total_milliseconds(); }
private:
  ptime duration_start_time;
  std::map<int, ptime> mduration_start_time;
};

int main(int argc, char *argv[])
{
  myTime mt;

  std::cout << "UTC : " << to_simple_string(mt.getCurrentTime()) << std::endl;

  mt.setStartDuration(0);
  Sleep(104);
  mt.setStartDuration(1);
  Sleep(52);
  std::cout << "duration(1) : " << mt.getDurdationMs(mt.getResultDuration(1)) << "ms" << std::endl;
  mt.setStartDuration(2);
  Sleep(82);
  std::cout << "duration(0) : " << mt.getDurdationMs(mt.getResultDuration(0)) << "ms" << std::endl;
  std::cout << "duration(2) : " << mt.getDurdationMs(mt.getResultDuration(2)) << "ms" << std::endl;

  return 0;
}

#if defined(DASH_ENABLE_PAPI)

#include <dash/util/internal/TimestampPAPI.h>

namespace dash {
namespace util {
namespace internal {

int TimestampPAPI<TimeMeasure::Clock>::timer_mode   = 0; 
int TimestampPAPI<TimeMeasure::Counter>::timer_mode = 0; 

} // namespace internal
} // namespace util
} // namespace dash

#endif // DASH_ENABLE_PAPI
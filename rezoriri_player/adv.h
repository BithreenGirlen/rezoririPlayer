#ifndef ADV_H_
#define ADV_H_

#include <string>

namespace adv
{
	struct TextDatum
	{
		std::string sceneText;
		std::string voiceFilePath;

		size_t nAnimationIndex = 0;
	};

	struct LabelDatum
	{
		std::string caption;
		size_t nTextIndex = 0;
	};
}
#endif // !ADV_H_

#include "Square.h"
#include <string>

std::string Common::Square::toString()
{
    std::string result;

	if (!this->isEmpty())
	{
		switch (pawn->player->getColor())
		{
		case 'R':
            result += RED;
			break;
		case 'G':
            result += GREEN;
			break;
		case 'B':
            result += BLUE;
			break;
		case 'Y':
            result += YELLOW;
			break;
		}
        result += pawn->number;
        result.append(" ").append(RESET);
	}
	else
	{
		switch (type)
		{
		case Common::SquareType::Path:
            result += ". ";
			break;
        case Common::SquareType::PathR:
            result.append(RED).append(". ");
            break;
        case Common::SquareType::PathG:
            result.append(GREEN).append(". ");
            break;
        case Common::SquareType::PathB:
            result.append(BLUE).append(". ");
            break;
        case Common::SquareType::PathY:
            result.append(YELLOW).append(". ");
            break;
		case Common::SquareType::HomeR:
            result.append(RED).append("r ");
			break;
		case Common::SquareType::HomeG:
            result.append(GREEN).append("g ");
			break;
		case Common::SquareType::HomeB:
            result.append(BLUE).append("b ");
			break;
		case Common::SquareType::HomeY:
            result.append(YELLOW).append("y ");
			break;
		case Common::SquareType::StartR:
            result.append(RED).append("# ");
			break;
		case Common::SquareType::StartG:
            result.append(GREEN).append("# ");
			break;
		case Common::SquareType::StartB:
            result.append(BLUE).append("# ");
			break;
		case Common::SquareType::StartY:
            result.append(YELLOW).append("# ");
			break;
		case Common::SquareType::None:
            result += "  ";
			break;
		}
        result += RESET;
	}

    return result;
}
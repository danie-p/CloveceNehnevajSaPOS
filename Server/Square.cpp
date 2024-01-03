#include "Square.h"
#include <string>

std::string Server::Square::toString()
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
		case Server::SquareType::Path:
            result += ". ";
			break;
        case Server::SquareType::PathR:
            result.append(RED).append(". ");
            break;
        case Server::SquareType::PathG:
            result.append(GREEN).append(". ");
            break;
        case Server::SquareType::PathB:
            result.append(BLUE).append(". ");
            break;
        case Server::SquareType::PathY:
            result.append(YELLOW).append(". ");
            break;
		case Server::SquareType::HomeR:
            result.append(RED).append("r ");
			break;
		case Server::SquareType::HomeG:
            result.append(GREEN).append("g ");
			break;
		case Server::SquareType::HomeB:
            result.append(BLUE).append("b ");
			break;
		case Server::SquareType::HomeY:
            result.append(YELLOW).append("y ");
			break;
		case Server::SquareType::StartR:
            result.append(RED).append("# ");
			break;
		case Server::SquareType::StartG:
            result.append(GREEN).append("# ");
			break;
		case Server::SquareType::StartB:
            result.append(BLUE).append("# ");
			break;
		case Server::SquareType::StartY:
            result.append(YELLOW).append("# ");
			break;
		case Server::SquareType::None:
            result += "  ";
			break;
		}
        result += RESET;
	}

    return result;
}
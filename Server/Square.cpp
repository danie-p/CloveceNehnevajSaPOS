#include "Square.h"
#include <iostream>

void Common::Square::display()
{
	if (!this->isEmpty())
	{
		switch (pawn->player->getColor())
		{
		case 'R':
			std::cout << RED;
			break;
		case 'G':
			std::cout << GREEN;
			break;
		case 'B':
			std::cout << BLUE;
			break;
		case 'Y':
			std::cout << YELLOW;
			break;
		default:
			break;
		}
		std::cout << pawn->number << " " << RESET;
	}
	else
	{
		switch (type)
		{
		case Common::SquareType::Path:
			std::cout << ". " << RESET;
			break;
		case Common::SquareType::HomeR:
			std::cout << RED << "r " << RESET;
			break;
		case Common::SquareType::HomeG:
			std::cout << GREEN << "g " << RESET;
			break;
		case Common::SquareType::HomeB:
			std::cout << BLUE << "b " << RESET;
			break;
		case Common::SquareType::HomeY:
			std::cout << YELLOW << "y " << RESET;
			break;
		case Common::SquareType::StartR:
			std::cout << RED << "# " << RESET;
			break;
		case Common::SquareType::StartG:
			std::cout << GREEN << "# " << RESET;
			break;
		case Common::SquareType::StartB:
			std::cout << BLUE << "# " << RESET;
			break;
		case Common::SquareType::StartY:
			std::cout << YELLOW << "# " << RESET;
			break;
		case Common::SquareType::None:
		default:
			std::cout << "  ";
			break;
		}
	}
};
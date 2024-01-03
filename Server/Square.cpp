#include "Square.h"
#include <iostream>

void Server::Square::display()
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
		case Server::SquareType::Path:
			std::cout << ". " << RESET;
			break;
		case Server::SquareType::HomeR:
			std::cout << RED << "r " << RESET;
			break;
		case Server::SquareType::HomeG:
			std::cout << GREEN << "g " << RESET;
			break;
		case Server::SquareType::HomeB:
			std::cout << BLUE << "b " << RESET;
			break;
		case Server::SquareType::HomeY:
			std::cout << YELLOW << "y " << RESET;
			break;
		case Server::SquareType::StartR:
			std::cout << RED << "# " << RESET;
			break;
		case Server::SquareType::StartG:
			std::cout << GREEN << "# " << RESET;
			break;
		case Server::SquareType::StartB:
			std::cout << BLUE << "# " << RESET;
			break;
		case Server::SquareType::StartY:
			std::cout << YELLOW << "# " << RESET;
			break;
		case Server::SquareType::None:
		default:
			std::cout << "  ";
			break;
		}
	}
};
#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <unistd.h>

// Function to split a string by a given delimiter
std::vector<std::string> splitString(std::string string, char delimiter)
{
  std::stringstream ss(string);
  std::vector<std::string> return_vector;
  std::string token;
  while (getline(ss, token, delimiter))
  {
    return_vector.push_back(token);
  }
  return return_vector;
}

// Function to handle the 'type' command
void handleTypeCommand(std::string command, std::string arguments, std::vector<std::string> path_vect)
{
  if (arguments == "exit" || arguments == "echo" || arguments == "type" || arguments == "pwd" || arguments == "cd")
  {
    std::cout << arguments << " is a shell builtin\n";
  }
  else
  {
    std::string filepath;
    for (int i = 0; i < path_vect.size(); i++)
    {
      filepath = path_vect[i] + '/' + arguments;
      std::ifstream file(filepath);
      if (file.good())
      {
        std::cout << arguments << " is " << filepath << "\n";
        return;
      }
    }
    std::cout << arguments << ": not found\n";
  }
}

int main()
{
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string path_string = std::getenv("PATH");
  std::vector<std::string> path_vector = splitString(path_string, ':');
  const char *home = std::getenv("HOME");

  while (true)
  {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    if (input == "exit 0")
    {
      return 0;
    }

    std::string command = input.substr(0, input.find(" "));
    std::string arguments = input.substr(input.find(" ") + 1);

    if (command == "echo")
    {
      std::cout << arguments << "\n";
    }
    else if (command == "type")
    {
      handleTypeCommand(command, arguments, path_vector);
    }
    else if (command == "pwd")
    {
      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)) != nullptr)
      {
        std::cout << cwd << "\n";
      }
      else
      {
        perror("getcwd() error");
      }
    }
    else if (command == "cd")
    {
      if (arguments == "~")
      {
        if (home != nullptr)
        {
          if (chdir(home) != 0)
          {
            std::cerr << "cd: " << home << ": No such file or directory\n";
          }
        }
        else
        {
          std::cerr << "cd: HOME environment variable not set\n";
        }
      }
      else
      {
        if (chdir(arguments.c_str()) != 0)
        {
          std::cerr << "cd: " << arguments << ": No such file or directory\n";
        }
      }
    }
    else
    {
      std::string filepath;
      bool found = false;
      for (int i = 0; i < path_vector.size(); i++)
      {
        filepath = path_vector[i] + '/' + command;
        std::ifstream file(filepath);
        if (file.good())
        {
          std::string exec_command = filepath + " " + arguments;
          std::system(exec_command.c_str());
          found = true;
          break;
        }
      }
      if (!found)
      {
        std::cout << command << ": not found\n";
      }
    }
  }
}

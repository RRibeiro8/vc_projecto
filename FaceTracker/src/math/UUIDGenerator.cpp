#include <string>

#ifndef UUID_GEN
#define UUID_GEN

using namespace std;

class UUIDGenerator
{
	public:
		static string generate()
		{
			string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
			string id = "";

			int random = 0, temp;

			for(int i = 0; i < 36; i++)
			{
				if(i == 8 || i == 13 || i == 18 || i == 23)
				{
					id += '-';
				}
				else
				{
					if(random <= 0x02)
					{
						random = randomFloat() * 0x1000000 + 0x2000000;
					}

					temp = random & 0xf;
					random = random >> 4;
					id += chars[(i == 19) ? (temp & 0x3) | 0x8 : temp];
				}
			}

			return id;
		}

		static float randomFloat()
		{
			return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		}
};

#endif

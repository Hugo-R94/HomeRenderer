#ifndef DATA_HPP
#define DATA_HPP

class Data
{
	private:
		int		_res_x;
		int		_res_y;
		
	public:
		Data();
		~Data();
		
		int			GetResx();
		int			GetResy();
		void		setResx(int resx);
		void		setResy(int resy);
};



#endif
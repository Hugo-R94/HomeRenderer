#include "HomeRenderer.hpp"
#include "threadpool.hpp"
#ifndef DATAGLOBAL_HPP
#define DATAGLOBAL_HPP

class DataGlobal {
private:
    int _res_x;
    int _res_y;
    int _meshID = 0;
	// std::vector<Mesh> grid;

public:
    DataGlobal() : _res_x(1920), _res_y(1080) {}
    DataGlobal(int resx, int resy) : _res_x(resx), _res_y(resy) {}
    ~DataGlobal() {}

    int getResx() const { return _res_x; }
    int getResy() const { return _res_y; }
    int getMeshID() const { return _meshID; }
    ThreadPool pool{ std::thread::hardware_concurrency() };

    void SetResx(int resx) { _res_x = resx; }
    void SetResy(int resy) { _res_y = resy; }
    void setMeshID(int meshID) { _meshID = meshID; }

    void setResx(int resx) { SetResx(resx); }
    void setResy(int resy) { SetResy(resy); }
    
};

#endif

// ChildView.cpp : implementation of the CChildView class
//
#include "mapbox/vector_tile.hpp"
 

#include "stdafx.h"
#include "mvtViewer.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

 ///////////////////////////////////////////////////
#include <string>
#include <vector>

#include <fstream>
#include <iostream>
#include <algorithm>



std::string open_tile(const char* path) {
	std::ifstream stream(path, std::ios_base::in | std::ios_base::binary);
	if (!stream.is_open())
	{
		return std::string();
	}
	std::string message(std::istreambuf_iterator<char>(stream.rdbuf()), (std::istreambuf_iterator<char>()));
	stream.close();
	return message;
}

class print_value {

public:
	std::string operator()(std::vector<mapbox::feature::value> val) {
		return "vector";
	}

	std::string operator()(std::unordered_map<std::string, mapbox::feature::value> val) {
		return "unordered_map";
	}

	std::string operator()(mapbox::feature::null_value_t val) {
		return "null";
	}

	std::string operator()(std::nullptr_t val) {
		return "nullptr";
	}

	std::string operator()(uint64_t val) {
		return std::to_string(val);
	}
	std::string operator()(int64_t val) {
		return std::to_string(val);
	}
	std::string operator()(double val) {
		return std::to_string(val);
	}
	std::string operator()(std::string const& val) {
		return val;
	}

	std::string operator()(bool val) {
		return val ? "true" : "false";
	}
};

std::vector<mapbox::vector_tile::points_arrays_type> geo_array;
//=============================



  
int LoadMvtFile(const char* pMvtFile)
{

	std::string buffer = open_tile(pMvtFile);
	mapbox::vector_tile::buffer tile(buffer);

	for (auto const& name : tile.layerNames())
	{
		const mapbox::vector_tile::layer layer = tile.getLayer(name);
		std::size_t feature_count = layer.featureCount();
		if (feature_count == 0) {
			std::cout << "Layer '" << name << "' (empty)\n";
			continue;
		}
		std::cout << "Layer '" << name << "'\n";
		std::cout << "  Features:\n";
		for (std::size_t i = 0; i < feature_count; ++i)
		{
			auto const feature = mapbox::vector_tile::feature(layer.getFeature(i), layer);
			auto const& feature_id = feature.getID();
			if (feature_id.is<uint64_t>()) {
				std::cout << "    id: " << feature_id.get<uint64_t>() << "\n";
			}
			else {
				std::cout << "    id: (no id set)\n";
			}
			std::cout << "    type: " << int(feature.getType()) << "\n";
			auto props = feature.getProperties();
			std::cout << "    Properties:\n";
			for (auto const& prop : props) {
				print_value printvisitor;
				std::string value = mapbox::util::apply_visitor(printvisitor, prop.second);
				std::cout << "      " << prop.first << ": " << value << "\n";
			}
			std::cout << "    Vertices:\n";
			mapbox::vector_tile::points_arrays_type geom = feature.getGeometries<mapbox::vector_tile::points_arrays_type>(0.125);


			if (feature.getType() == 2)
			{
				geo_array.push_back(geom);
			}

			for (auto const& point_array : geom) {
				for (auto const& point : point_array) {
					std::clog << point.x << "," << point.y << ";";
				}
			}
		}// feature for end;
	}

	return true;
}
CChildView::CChildView()
{

	const char* pMvtFile = "E:/01_vpn/13457_6237_14.mvt";
	//const char* pMvtFile = "E:/01_vpn/8_20_5.mvt";

	LoadMvtFile(pMvtFile);

}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	for (auto const& geom : geo_array)
	{
		for (auto const& point_array : geom)
		{
			for (size_t i = 0; i < point_array.size(); i++)
			{
				auto point = point_array[i];
				if (i != 0)
				{
					dc.LineTo(point.x, point.y);
				}
				else
				{
					dc.MoveTo(point.x, point.y);
				}
			}
		}
	}
	// TODO: Add your message handler code here

	// Do not call CWnd::OnPaint() for painting messages
}


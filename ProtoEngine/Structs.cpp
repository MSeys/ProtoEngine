#include "ProtoEnginePCH.h"
#include "Structs.h"

void TextureFrame::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* frameNode = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<TextureFrame>().c_str()));
	ProtoSaver::XML::Save<float>("DstPositionX", dstPosition.x, doc, frameNode);
	ProtoSaver::XML::Save<float>("DstPositionY", dstPosition.y, doc, frameNode);

	ProtoSaver::XML::Save<float>("DstSizeX", dstSize.x, doc, frameNode);
	ProtoSaver::XML::Save<float>("DstSizeY", dstSize.y, doc, frameNode);

	ProtoSaver::XML::Save<float>("SrcPositionX", srcPosition.x, doc, frameNode);
	ProtoSaver::XML::Save<float>("SrcPositionY", srcPosition.y, doc, frameNode);

	ProtoSaver::XML::Save<float>("SrcSizeX", srcSize.x, doc, frameNode);
	ProtoSaver::XML::Save<float>("SrcSizeY", srcSize.y, doc, frameNode);

	ProtoSaver::XML::Save<float>("ScaleX", scale.x, doc, frameNode);
	ProtoSaver::XML::Save<float>("ScaleY", scale.y, doc, frameNode);

	ProtoSaver::XML::Save<float>("PivotX", pivot.x, doc, frameNode);
	ProtoSaver::XML::Save<float>("PivotY", pivot.y, doc, frameNode);

	ProtoSaver::XML::Save<Uint8>("ColorR", color.r, doc, frameNode);
	ProtoSaver::XML::Save<Uint8>("ColorG", color.g, doc, frameNode);
	ProtoSaver::XML::Save<Uint8>("ColorB", color.b, doc, frameNode);
	ProtoSaver::XML::Save<Uint8>("ColorA", color.a, doc, frameNode);
	
	pParent->append_node(frameNode);
}

TextureFrame TextureFrame::Load(rapidxml::xml_node<>* pComp)
{
	TextureFrame frame;
	frame.dstPosition = {
		ProtoParser::XML::Parse<float>(pComp, "DstPositionX"),
		ProtoParser::XML::Parse<float>(pComp, "DstPositionY")
	};

	frame.dstSize = {
		ProtoParser::XML::Parse<float>(pComp, "DstSizeX"),
		ProtoParser::XML::Parse<float>(pComp, "DstSizeY")
	};
	
	frame.srcPosition = {
		ProtoParser::XML::Parse<float>(pComp, "SrcPositionX"),
		ProtoParser::XML::Parse<float>(pComp, "SrcPositionY")
	};

	frame.srcSize = {
		ProtoParser::XML::Parse<float>(pComp, "SrcSizeX"),
		ProtoParser::XML::Parse<float>(pComp, "DstSizeY")
	};

	frame.scale = {
		ProtoParser::XML::Parse<float>(pComp, "ScaleX"),
		ProtoParser::XML::Parse<float>(pComp, "ScaleY")
	};

	frame.pivot = {
		ProtoParser::XML::Parse<float>(pComp, "PivotX"),
		ProtoParser::XML::Parse<float>(pComp, "PivotY")
	};

	frame.color = {
		ProtoParser::XML::Parse<Uint8>(pComp, "ColorR"),
		ProtoParser::XML::Parse<Uint8>(pComp, "ColorG"),
		ProtoParser::XML::Parse<Uint8>(pComp, "ColorB"),
		ProtoParser::XML::Parse<Uint8>(pComp, "ColorA")
	};

	return frame;
}

#include "stdafx.h"
#include "PolygonClip.h"

PolygonLineClip::PolygonLineClip(Boundary* pBoundary, Line* pLine) {
	this->pBoundary = pBoundary;
	this->pLine = pLine;
	//��֤endPoint��y��startPoint��y��
	if (pLine->endpoint.y < pLine->startpoint.y) {
		swap(pLine->startpoint.x, pLine->endpoint.x);
		swap(pLine->startpoint.y, pLine->endpoint.y);
	}
}

PolygonLineClip::~PolygonLineClip(void) {
}

BOOL PolygonLineClip::GetClipResult(ClipLineResult &result) {
	size_t vetexSize = pBoundary->vertexs.size();
	/*
	if(vetexSize<4 || pBoundary->vertexs[vetexSize-1] != pBoundary->vertexs[0])
	return FALSE;*/
	result.intersection.clear();

	result.relation = Relation::Outside;
	double lx1 = pLine->startpoint.x, lx2 = pLine->endpoint.x, ly1 = pLine->startpoint.y, ly2 = pLine->endpoint.y;
	//�ж���ȫ���ɼ����
	if (min(lx1, lx2) > pBoundary->rightPoint.x ||
		max(lx1, lx2) < pBoundary->leftPoint.x ||
		min(ly1, ly2) > pBoundary->topPoint.y ||
		max(ly1, ly2) < pBoundary->bottomPoint.y)
	{
		//��ȫ���ɼ�
		result.relation = Relation::Outside;
		return TRUE;
	}

	//��ȡ�߶���y������н���任�����е�u,v
	double angle_rad;
	angle_rad = atan2(lx2 - lx1, ly2 - ly1);
	double cos_angle = cos(angle_rad);
	double sin_angle = sin(angle_rad);

	//��ʼ����仯
	Point Line(lx2, ly2);
	Point LineTrans;
	LineTrans.x = (lx2 - lx1)*cos_angle - (ly2 - ly1)*sin_angle;
	LineTrans.y = (lx2 - lx1)*sin_angle + (ly2 - ly1)*cos_angle;
	vector<double> interYs;//����Y����
	interYs.reserve(24);

	for (unsigned int i = 0; i < vetexSize - 1; i++) {
		//ת���߽��
		Point P1(pBoundary->vertexs[i].x, pBoundary->vertexs[i].y), P2(pBoundary->vertexs[i + 1].x, pBoundary->vertexs[i + 1].y);
		Point P1Trans, P2Trans;
		P1Trans.x = (P1.x - lx1)*cos_angle - (P1.y - ly1)*sin_angle;
		P1Trans.y = (P1.x - lx1)*sin_angle + (P1.y - ly1)*cos_angle;
		P2Trans.x = (P2.x - lx1)*cos_angle - (P2.y - ly1)*sin_angle;
		P2Trans.y = (P2.x - lx1)*sin_angle + (P2.y - ly1)*cos_angle;
		//�ж���Ч�߽���
		if ((P1Trans.x>0 && P2Trans.x > 0) || (P1Trans.x < 0 && P2Trans.x<0) || min(P1Trans.y, P2Trans.y)>LineTrans.y){	
			// || max(P1Trans.y, P2Trans.y) < 0) {
			//�ñ߽���Ϊ��Ч�߽���
			//cout<<"get invalid "<<mxP1<<" "<<mxP2<<endl;
		}
		else {
			//��Ϊ����
			if (ISZERO(P1Trans.x) && ISZERO(P2Trans.x)) {
				//�����������Ĵ����ߵ����
				//���߶��ӳ����·�
				if (max(P1Trans.y, P2Trans.y) < 0) {
					interYs.push_back(max(P1Trans.y, P2Trans.y));
				}
			}
			else//��Ϊ����
			{
				//��ý���
				double kxTrans = 0;
				double kyTrans = P2Trans.y + P2Trans.x*(P1Trans.y - P2Trans.y) / (P2Trans.x - P1Trans.x);

				if (kyTrans <= LineTrans.y && kyTrans >= 0)
					interYs.push_back(kyTrans);
			}
		}
	}

	interYs.push_back(0);
	interYs.push_back(LineTrans.y);
	sort(interYs.begin(), interYs.end());

	Point K1;
	Point K2;
	Point midPoint;
	result.linesClipped.clear();

	if (interYs.size() > 2) {
		for (unsigned int j = 0; j < interYs.size() - 1; j++) {
			if (interYs[j]<0 || interYs[j]> LineTrans.y)
				continue;
			K1.x = interYs[j] * sin_angle + lx1;
			K1.y = interYs[j] * cos_angle + ly1;
			K2.x = interYs[j + 1] * sin_angle + lx1;
			K2.y = interYs[j + 1] * cos_angle + ly1;
			if (!ISZERO(interYs[j]) && !ISZERO(interYs[j] - LineTrans.y))
				result.intersection.push_back(K1);
			midPoint.x = (K1.x + K2.x) / 2.0;
			midPoint.y = (K1.y + K2.y) / 2.0;
			if (PolygonCircleClip::IsPointInside(pBoundary, midPoint)) {
				result.relation = Intersect;
				LineClipped lc;
				lc.startPoint = K1; lc.endPoint = K2;				
				result.linesClipped.push_back(lc);
			}
		}
	}
	else
		//if(result.relation != Intersect)
	{
		K1 = pLine->startpoint; K2 = pLine->endpoint;		
		midPoint.x = (K1.x + K2.x) / 2.0;
		midPoint.y = (K1.y + K2.y) / 2.0;
		if (PolygonCircleClip::IsPointInside(pBoundary, midPoint)) {
			result.relation = Inside;
		}
		else {
			result.relation = Outside;
		}

	}
	return TRUE;
}

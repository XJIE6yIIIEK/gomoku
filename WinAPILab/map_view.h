#pragma once

namespace mapView {
	class MapView {
		public:
			HANDLE hMapField;
			LPCTSTR lpMap;
			TCHAR* szMapName;
			int fileBufSize;

			bool initSync = false;

			MapView(std::string, int);

			~MapView();

			virtual void SyncFromMapView() = 0;
			virtual void SyncToMapView() = 0;
	};

	class FieldMapView : MapView {
		public:
			FieldMapView(std::string, int);
			~FieldMapView();

			void SyncFromMapView();

			void SyncToMapView();
	};

	class GamestateMapView : MapView {
		public:
			GamestateMapView(std::string, int);

			~GamestateMapView();

			void SyncFromMapView();
			void SyncToMapView();
	};
}

namespace saveFileHandler {
	void LoadSave();
	void SaveGame();
}

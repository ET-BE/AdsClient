template <typename T>
ADSBase<T>::ADSBase()
{

}

template <typename T>
ADSBase<T>::~ADSBase()
{

}

template <typename T>
void ADSBase<T>::connect(const int& port)
{
	client_ = new tcAdsClient(port);
}

template <typename T>
void ADSBase<T>::disconnect()
{
	nErr_ = AdsPortClose();
	if (nErr_) 
		std::cout << "ERROR: AdsPortClose: " << nErr_ << std::endl << std::flush;
}

template <typename T>
void ADSBase<T>::send(std::vector<T>& data, const unsigned long& nIndexGroup, const unsigned long& nIndexOffset)
{
	//std::vector<DWORD> dwData;
	//unsigned long size = ceil(data.size() * sizeof(T) / sizeof(DWORD));
	//dwData.resize(size);

	//memcpy(dwData.data(), data.data(), data.size() * sizeof(T));

	nErr_ = AdsSyncWriteReq(&Addr_, nIndexGroup, nIndexOffset, data.size(), data.data());
	if (nErr_)
		std::cout << "ERROR: AdsSyncWriteReq: " << nErr_ << std::endl << std::flush;
}

template <typename T>
std::vector<T> ADSBase<T>::receive(const int& length, const unsigned long& nIndexGroup, const unsigned long& nIndexOffset)
{
	std::vector<T> data;
	data.resize(length);

	/*std::vector<DWORD> dwData;
	unsigned long size = ceil(length * sizeof(T) / sizeof(DWORD));
	dwData.resize(size);*/

	nErr_ = AdsSyncReadReq(&Addr_, nIndexGroup, nIndexOffset, data.size(), data.data());
	if (nErr_)
		std::cout << "ERROR: AdsSyncReadReq: " << nErr_ << std::endl << std::flush;

	//memcpy(data.data(), dwData.data(), length * sizeof(T));

	return data;
}

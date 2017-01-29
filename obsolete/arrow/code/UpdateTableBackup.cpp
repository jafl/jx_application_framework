if (ok)
	{
	JPtrArray<GMessageFrom> fromlist;
	fromlist.SetCompareFunction(GMessageFrom::CompareFromLines);
	JSize count = list->GetElementCount();
	JSize index;
	for (index = 1; index <= count; index++)
		{
		GMessageHeader* header = list->GetElement(index);
		GMessageFrom* from = new GMessageFrom(header);
		assert(from != NULL);
		fromlist.InsertSorted(from, kJTrue);
		}
	count = itsFromList->GetElementCount();
	for (index = count; index >= 1; index--)
		{
		GMessageFrom* from = itsFromList->GetElement(index);
		JIndex findindex;
		if (!fromlist.SearchSorted(from, JListT::kFirstMatch, &findindex))
			{
			GMessageHeader* header = from->GetHeader();
			itsList->Remove(header);
			CloseMessage(header);
			itsFromList->Remove(from);
			delete header;
			delete from;
			}
		}
	count = fromlist.GetElementCount();
	for (index = count; index >= 1; index--)
		{
		GMessageFrom* from = fromlist.GetElement(index);
		JIndex findindex;
		if (itsFromList->SearchSorted(from, JListT::kFirstMatch, &findindex))
			{
			GMessageFrom* currentfrom = itsFromList->GetElement(findindex);
			GMessageHeader* header = currentfrom->GetHeader();
			GMessageHeader* header2 = from->GetHeader();
			header->SetHeaderStart(header2->GetHeaderStart());
			header->SetHeaderEnd(header2->GetHeaderEnd());
			header->SetMessageEnd(header2->GetMessageEnd());
			list->Remove(header2);
			itsList->Remove(header2);
			CloseMessage(header2);
			fromlist.Remove(from);
			delete header2;
			delete from;
			}
		}
	count = fromlist.GetElementCount();
	for (index = 1; index <= count; index++)
		{
		GMessageFrom* from = fromlist.GetElement(index);
		GMessageHeader* header = from->GetHeader();
		itsList->Append(header);
		itsFromList->InsertSorted(from, kJTrue);
		}
	}

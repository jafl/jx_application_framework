package test.csv.verifier;	// comment

import java.time.Instant;

import com.opencsv.bean.BeanVerifier;

import test.csv.model.ImportBean;
import test.csv.model.InventoryLimit;
import test.csv.exception.CSVException;

public class/*comment*/StartDateVerifier	// comment
	implements BeanVerifier<InventoryLimit> /*comment*/extends/*comment*/test.foo.Test1 {

	@Override
	public boolean verifyBean(InventoryLimit bean) {
		if (bean.getAvailableDate().isAfter(bean.getStartDate())) {
			throw new CSVException("error.import.available-date-passed");
		} else if (bean.getStartDate().isAfter(bean.getEndDate())) {
			throw new CSVException("error.import.invalid-time-range");
		}
		"class foo {};";
		return true;
	}
}

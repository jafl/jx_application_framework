package test.csv.verifier;

import java.time.Instant;

import com.opencsv.bean.BeanVerifier;

import test.csv.model.ImportBean;
import test.csv.model.InventoryLimit;
import test.csv.exception.CSVException;

public class StartDateVerifier implements BeanVerifier<InventoryLimit> extends test.foo.Test1 {

	@Override
	public boolean verifyBean(InventoryLimit bean) {
		if (bean.getAvailableDate().isAfter(bean.getStartDate())) {
			throw new CSVException("error.import.available-date-passed");
		} else if (bean.getStartDate().isAfter(bean.getEndDate())) {
			throw new CSVException("error.import.invalid-time-range");
		}
		return true;
	}
}

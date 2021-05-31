package test.csv.verifier;

import java.time.Instant;

import com.opencsv.bean.BeanVerifier;

import test.csv.model.ImportBean;
import test.csv.exception.CSVException;

public class DateVerifier<T extends ImportBean> implements BeanVerifier<T> {

	private boolean canUpdateInvariant;

	public DateVerifier(boolean canUpdateInvariant) {
		this.canUpdateInvariant = canUpdateInvariant;
	}

	@Override
	public boolean verifyBean(T bean) {
		if (!canUpdateInvariant && bean.getEffectiveDate().isBefore(Instant.now())) {
			throw new CSVException("error.import.date-passed");
		}
		return true;
	}
}

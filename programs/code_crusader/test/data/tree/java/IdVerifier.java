package test.csv.verifier;

import com.opencsv.bean.BeanVerifier;

import test.csv.model.ImportBean;
import test.csv.exception.CSVException;

public class IdVerifier<T extends ImportBean> implements BeanVerifier<T> {

	private boolean canUpdateInvariant;

	public IdVerifier(boolean canUpdateInvariant) {
		this.canUpdateInvariant = canUpdateInvariant;
	}

	@Override
	public boolean verifyBean(T bean) {
		if (bean.isUpdateInvariant() && !canUpdateInvariant) {
			throw new CSVException("error.import.forbidden");
		} else if (bean.isUpdateInvariant() &&
					(bean.getRowId() == null || bean.getDataRowId() == null)) {
			throw new CSVException("error.import.missing-row-id");
		}
		return true;
	}
}
